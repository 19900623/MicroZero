#include "stdinc.h"
#include "net_request.h"
#include "net_command.h"


using namespace std;
namespace Agebull
{
	namespace Rpc
	{
		/**
		* @brief ��Ϣ��
		* @param {PNetCommand} cmd �������
		*/
		PNetCommand CommandPump::message_pump(NetCommandArgPtr& cmd_arg)
		{
			cmd_arg->cmd_state = NET_COMMAND_STATE_SUCCEED;
			return cmd_arg.m_command;
		}
		void CommandPump::request_pump()
		{
			log_msg1("����������(%s)��������", m_address);
			ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_REP);
			if (socket == nullptr)
			{
				auto err = zmq_strerror(errno);
				log_error2("����SOCKET����(%s)��������:%s", m_address, err);
				return;
			}

			int iZMQ_IMMEDIATE = 1;//����Ϣֻ����������ɵ�����
			zmq_setsockopt(socket, ZMQ_LINGER, &iZMQ_IMMEDIATE, sizeof(int));
			int iLINGER = 50;//�ر�����ͣ��ʱ��,����
			zmq_setsockopt(socket, ZMQ_LINGER, &iLINGER, sizeof(int));
			int iRcvTimeout = 500;
			zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(int));
			int iSndTimeout = 500;
			zmq_setsockopt(socket, ZMQ_SNDTIMEO, &iSndTimeout, sizeof(int));
			auto zmq_result = zmq_bind(socket, m_address);
			if (zmq_result < 0)
			{
				auto err = zmq_strerror(errno);
				log_error2("�󶨶˿�(%s)��������:%s", m_address, err);
				return;
			}
			log_msg1("����������(%s)������", m_address);
			//�Ǽ��߳̿�ʼ
			int state = 0;
			set_command_thread_start();
			RedisLiveScope redis_live_scope;
			while (get_net_state() == NET_STATE_RUNING)
			{
				//������������
				zmq_msg_t msg_call;
				zmq_result = zmq_msg_init(&msg_call);
				if (zmq_result != 0)
				{
					state = 2;//������
					break;
				}
				zmq_result = zmq_msg_recv(&msg_call, socket, 0);
				if (zmq_result <= 0)
				{
					switch (errno)
					{
					case ETERM:
						log_error1("�������(%s)����[��ָ����socket�������context���ս���],�Զ��ر�", m_address);
						state = 1;
						break;
					case ENOTSOCK:
						log_error1("�������(%s)����[ָ����socket������],�Զ�����", m_address);
						state = 2;
						break;
					case EINTR:
						log_error1("�������(%s)����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", m_address);
						state = 2;
						break;
					case EAGAIN:
						//ʹ�÷�������ʽ������Ϣ��ʱ��û�н��յ��κ���Ϣ��
					default:
						state = 0;
						break;
					}
					zmq_msg_close(&msg_call);
					if (state > 0)
						break;
					continue;
				}
				size_t len = zmq_msg_size(&msg_call);
				log_debug2(DEBUG_REQUEST, 6, "��������(%s)�ɹ�:%d", m_address, len);
				NetCommandArgPtr ptr(zmq_result);
				memcpy(ptr.m_buffer, static_cast<PNetCommand>(zmq_msg_data(&msg_call)), len);
				zmq_msg_close(&msg_call);

				if (!check_crc(ptr.m_command))
				{
					log_error1("���յ��Ƿ�����(%s),����֪ͨ�ط�", m_address);
					ptr->cmd_state = NET_COMMAND_STATE_ARGUMENT_INVALID;
					write_crc(ptr.m_command);
					zmq_result = zmq_send(socket, ptr.m_buffer, len, ZMQ_DONTWAIT);
				}
				else
				{
					log_debug4(DEBUG_REQUEST, 6, "(%s)���յ�����,��ַ%s,����%d(%s)", m_address, ptr->user_token, ptr->cmd_id, ptr->cmd_identity);
					//������
					PNetCommand result = message_pump(ptr);
					//�������ط�
					get_cmd_len(result);
					write_crc(result);
					//zmq_send(socket, cmd_call->user_token, strlen(cmd_call->user_token), ZMQ_SNDMORE);
					//zmq_send(socket, empty, 0, ZMQ_SNDMORE);
					zmq_result = zmq_send(socket, ptr.m_buffer, len, ZMQ_DONTWAIT);
				}

				if (zmq_result <= 0)
				{
					switch (errno)
					{
					case ETERM:
						log_error1("���ͻ�ִ(%s)����[��ָ����socket�������context���ս���],�Զ��ر�", m_address);
						state = 1;
						break;
					case ENOTSOCK:
						log_error1("���ͻ�ִ(%s)����[ָ����socket������],�Զ�����", m_address);
						state = 2;
						break;
					case EINTR:
						log_error1("���ͻ�ִ(%s)����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", m_address);
						state = 2;
						break;
					case EAGAIN:
						//ʹ�÷�������ʽ������Ϣ��ʱ��û�н��յ��κ���Ϣ��
					default:
						state = 0;
						continue;
					}
					break;
				}
				log_debug1(DEBUG_REQUEST, 6, "���ͻ�ִ(%s)�ɹ�", m_address);
			}
			zmq_unbind(socket, m_address);
			zmq_close(socket);
			//�Ǽ��̹߳ر�
			set_command_thread_end();
			if (state == 2 && get_net_state() == NET_STATE_RUNING)
			{
				log_msg("������������������");
				boost::thread thrds(boost::bind(&server_request));
			}
			else
			{
				log_msg1("����������(%s)�ѹر�", m_address);
			}
		}
	}
}
