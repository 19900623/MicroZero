#include "stdafx.h"
#include "debug/TraceStack.h"

#include <InnerTrade\MqCommand.h>
typedef Agebull::Rpc::MqCommand CommandProxy;

using namespace std;



//S�˵�������ö���
queue<NetCommandArgPtr> server_cmd_queue;
//S��������ö�����
boost::mutex server_cmd_mutex;


#ifdef COMMANDPROXY
//�������Ϣ��
void server_message_pump()
{
	log_msg("�������Ϣ����������");
	CommandProxy proxy;
	if (!proxy.Initialize())
	{
		log_msg("�������Ϣ�ó�ʼ��ʧ��,��������...");
		boost::thread thrds(boost::bind(&server_message_pump));
		return;
	}
	//server_msg_queue.emplace();
	//{
	//	boost::lock_guard<boost::mutex> guard(server_cmd_mutex);
	//	int size = server_cmd_queue.size();
	//	for (int i = 0;i < size;i++)
	//	{
	//		server_cmd_queue.pop();
	//	}
	//}
	log_msg("����˴������ӳɹ�");
	int state = 0;
	log_msg("�������Ϣ��������");
	while (get_net_state() != NET_STATE_DISTORY)
	{
		if (!proxy.succeed())
		{
			state = 2;
			break;
		}
		if (server_cmd_queue.empty())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			continue;
		}
		NetCommandArgPtr cmd_call;
		{
			boost::lock_guard<boost::mutex> guard(server_cmd_mutex);
			cmd_call = server_cmd_queue.front();
			server_cmd_queue.pop();
		}
		RedisTransScope scope;
		proxy.message_pump(cmd_call);
	}
	proxy.close();
	if (state == 2 && get_net_state() != NET_STATE_DISTORY)
	{
		log_error("����˴����쳣�ر�,�������´�...");
		boost::thread thrds(boost::bind(&server_message_pump));
	}
	else
	{
		log_msg("����˴����ѹر�");
	}
}
#endif

void server_request()
{
	string addr = config::get_config("request_add");
	char address[128];
	strcpy_s(address, addr.c_str());
	log_msg1("����������(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_REP);
	if (socket == nullptr)
	{
		auto err = zmq_strerror(errno);
		log_error2("����SOCKET����(%s)��������:%s", address, err);
		return;
	}
	int zmq_result;
	//zmq_result = zmq_socket_monitor(socket, "inproc://server_cmd.rep", ZMQ_EVENT_ALL);
	//assert(zmq_result == 0);
	int iZMQ_IMMEDIATE = 1;//����Ϣֻ����������ɵ�����
	zmq_setsockopt(socket, ZMQ_LINGER, &iZMQ_IMMEDIATE, sizeof(int));
	int iLINGER = 50;//�ر�����ͣ��ʱ��,����
	zmq_setsockopt(socket, ZMQ_LINGER, &iLINGER, sizeof(int));
	int iRcvTimeout = 500;
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(int));
	int iSndTimeout = 500;
	zmq_setsockopt(socket, ZMQ_SNDTIMEO, &iSndTimeout, sizeof(int));
	zmq_result = zmq_bind(socket, address);
	if (zmq_result < 0)
	{
		auto err = zmq_strerror(errno);
		log_error2("�󶨶˿�(%s)��������:%s", address, err);
		return;
	}
	log_msg1("����������(%s)������", address);
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
				log_error1("�������(%s)����[��ָ����socket�������context���ս���],�Զ��ر�", address);
				state = 1;
				break;
			case ENOTSOCK:
				log_error1("�������(%s)����[ָ����socket������],�Զ�����", address);
				state = 2;
				break;
			case EINTR:
				log_error1("�������(%s)����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", address);
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
		log_debug2(DEBUG_REQUEST, 6, "��������(%s)�ɹ�:%d", address, len);
		NetCommandArgPtr ptr(zmq_result);
		memcpy(ptr.m_buffer, static_cast<PNetCommand>(zmq_msg_data(&msg_call)), len);
		zmq_msg_close(&msg_call);
		char result[1];
		if (!check_crc(ptr.m_command))//(ptr.m_buffer[0] != '$' || (len > sizeof(NetCommand) &&  ptr.m_buffer[len - 1] != OBJ_TYPE_END))
		{
			log_error1("���յ�δ������(%s),����֪ͨ�ط�", address);
			result[0] = '0';
		}
		else
		{
			log_debug4(DEBUG_REQUEST, 6, "(%s)���յ�����,�û�%s,����%d(%s)", address, ptr->user_token, ptr->cmd_id, ptr->cmd_identity);
			result[0] = '1';
		}
		//�������ѽ��ջط�
		zmq_result = zmq_send(socket, result, 1, ZMQ_DONTWAIT);
		if (zmq_result <= 0)
		{
			switch (errno)
			{
			case ETERM:
				log_error1("���ͻ�ִ(%s)����[��ָ����socket�������context���ս���],�Զ��ر�", address);
				state = 1;
				break;
			case ENOTSOCK:
				log_error1("���ͻ�ִ(%s)����[ָ����socket������],�Զ�����", address);
				state = 2;
				break;
			case EINTR:
				log_error1("���ͻ�ִ(%s)����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", address);
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
		if (result[0] == '0')
		{
			log_debug1(DEBUG_REQUEST, 1, "�����ط���ִ(%s)�ɹ�", address);
			continue;
		}
		log_msg1("��Ϣ����(%s)", ptr->cmd_identity);
		log_debug1(DEBUG_REQUEST, 6, "���ͻ�ִ(%s)�ɹ�", address);
		//������������
		push_cmd(ptr);
		//׼����һ�η�������(�����������ѽ���)
		//publish_command_state(ptr, NET_COMMAND_STATE_WAITING);
	}
	zmq_unbind(socket, address);
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
		log_msg1("����������(%s)�ѹر�", address);
	}
}
void push_cmd(NetCommandArgPtr& call)
{
	//д�빤������
	boost::lock_guard<boost::mutex> guard(server_cmd_mutex);
	server_cmd_queue.push(NetCommandArgPtr(call));
}

void push_cmd(PNetCommand call_cpy)
{
	boost::lock_guard<boost::mutex> guard(server_cmd_mutex);
	server_cmd_queue.push(NetCommandArgPtr(call_cpy));
}


//������Ϣ����
queue<NetCommandArgPtr> server_answer_queue;
//������Ϣ������
boost::mutex server_answer_mutex;

//������Ϣ����
queue<NetCommandArgPtr> server_notify_queue;
//������Ϣ������
boost::mutex server_notify_mutex;

//�������Ϣ����
void command_answer(PNetCommand cmd)
{
	NetCommandArgPtr ptr(cmd);
	command_answer(ptr);
}
//�������Ϣ����
void command_answer(NetCommandArgPtr& cmd)
{

	log_error1("��Ϣ����\r\n%s", cmd->user_token);

	boost::lock_guard<boost::mutex> guard(server_answer_mutex);
	server_answer_queue.push(NetCommandArgPtr(cmd));
}
//������첽�ط�
void server_answer()
{
	const string address = config::get_config("answer_add");
	log_msg1("���������ط�(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_PUB);
	if (socket == nullptr)
	{
		log_error2("����SOCKET����(%s)��������:%s", address, zmq_strerror(errno));
		return;
	}
	int zmq_result /*= zmq_socket_monitor(socket, "inproc://server_pub.rep", ZMQ_EVENT_ALL);
				   assert(zmq_result == 0)*/;
	zmq_result = zmq_bind(socket, address.c_str());
	if (zmq_result < 0)
	{
		log_error2("�󶨶˿�(%s)��������:%s", address, zmq_strerror(errno));
		return;
	}
	//const char empty[2] = "";
	log_msg1("���������ط�(%s)������", address);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	while (get_net_state() == NET_STATE_RUNING)
	{
		if (server_answer_queue.empty())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			continue;
		}
		NetCommandArgPtr cmd_call;
		{
			boost::lock_guard<boost::mutex> guard(server_answer_mutex);
			cmd_call = server_answer_queue.front();
			server_answer_queue.pop();
			if (cmd_call.m_buffer == nullptr)
				continue;
		}
		log_msg2("��Ϣ����***\r\n%s:::%d", cmd_call->user_token, cmd_call->cmd_state);
		int len = get_cmd_len(cmd_call);
		write_crc(cmd_call.m_command);
		//zmq_send(socket, cmd_call->user_token, strlen(cmd_call->user_token), ZMQ_SNDMORE);
		//zmq_send(socket, empty, 0, ZMQ_SNDMORE);
		zmq_result = zmq_send(socket, cmd_call.m_buffer, len, ZMQ_DONTWAIT);

		if (zmq_result <= 0)
		{
			int error = errno;
			if (error != 0)
			{
				thread_sleep(1);
				log_error2("������Ϣ(%s)ʱ��������(%s)", address, zmq_strerror(errno));
			}
			continue;
		}
#ifdef _DEBUG
		log_trace4(DEBUG_SUB, 3, "(%s)����(%d)״̬(%d)֪ͨ��%s", address, cmd_call->cmd_id, cmd_call->cmd_state, cmd_call->user_token);
#endif
	}
	zmq_unbind(socket, address.c_str());
	zmq_close(socket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
	log_msg1("���������ط�(%s)�ѹر�", address);
}
//����˹㲥����
void notify_message(PNetCommand cmd)
{
	NetCommandArgPtr ptr(cmd);
	notify_message(ptr);
}
//����˹㲥����
void notify_message(NetCommandArgPtr& cmd)
{
	boost::lock_guard<boost::mutex> guard(server_notify_mutex);
	server_notify_queue.push(NetCommandArgPtr(cmd));
}
//�������Ϣ�㲥
void server_notify()
{
	const string address = config::get_config("notify_add");
	log_msg1("�������Ϣ�㲥(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_PUB);
	if (socket == nullptr)
	{
		log_error2("����SOCKET����(%s)��������:%s", address, zmq_strerror(errno));
		return;
	}
	int zmq_result /*= zmq_socket_monitor(socket, "inproc://server_pub.rep", ZMQ_EVENT_ALL);
				   assert(zmq_result == 0)*/;
	zmq_result = zmq_bind(socket, address.c_str());
	if (zmq_result < 0)
	{
		log_error2("�󶨶˿�(%s)��������:%s", address, zmq_strerror(errno));
		return;
	}
	//const char empty[2] = "";
	log_msg1("�������Ϣ�㲥(%s)������", address);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	while (get_net_state() == NET_STATE_RUNING)
	{
		if (server_notify_queue.empty())
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			continue;
		}
		NetCommandArgPtr cmd_call;
		{
			boost::lock_guard<boost::mutex> guard(server_notify_mutex);
			cmd_call = server_notify_queue.front();
			server_notify_queue.pop();
			if (cmd_call.m_buffer == nullptr)
				continue;
		}
		int len = get_cmd_len(cmd_call);
		write_crc(cmd_call.m_command);
		//zmq_send(socket, cmd_call->user_token, strlen(cmd_call->user_token), ZMQ_SNDMORE);
		//zmq_send(socket, empty, 0, ZMQ_SNDMORE);
		zmq_result = zmq_send(socket, cmd_call.m_buffer, len, ZMQ_DONTWAIT);

		if (zmq_result <= 0)
		{
			int error = errno;
			if (error != 0)
			{
				thread_sleep(1);
				log_error2("������Ϣ(%s)ʱ��������(%s)", address, zmq_strerror(errno));
			}
			continue;
		}
		log_msg2("��Ϣ�㲥(%s-%s)", cmd_call->cmd_identity, cmd_call->user_token);
#ifdef _DEBUG
		log_trace4(DEBUG_SUB, 3, "(%s)����(%d)״̬(%d)֪ͨ��%s", address, cmd_call->cmd_id, cmd_call->cmd_state, cmd_call->user_token);
#endif
	}
	zmq_unbind(socket, address.c_str());
	zmq_close(socket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
	log_msg1("�������Ϣ�㲥(%s)�ѹر�", address);
}