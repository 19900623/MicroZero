/**
 * ZMQ�㲥������
 *
 *
 */

#include "stdafx.h"
#include "BroadcastingStation.h"
#include "SysCmdArg/StringArgument.h"
namespace agebull
{
	namespace zmq_net
	{
		map<string, shared_ptr<BroadcastingStation>> BroadcastingStation::examples;

#define send_result(code,type,msg,addr)\
	log_error3("%s(%s),����%s",msg, addr,code);\
	response[0] = type;\
	result = zmq_send(_outSocket, response, 1, ZMQ_DONTWAIT);\
	if(result <= 0)\
	{\
		if (!check_zmq_error(_zmq_state, request_address, code))\
			break;\
		continue;\
	}\
	log_debug2(DEBUG_REQUEST, 6, "%(%s)�ɹ�",code, request_address);

		/**
		*��Ϣ��
		*/
		bool BroadcastingStation::publish(string name, string type, string arg)
		{
			auto pub = examples.find(name);
			if (pub == examples.end() || !pub->second->can_do())
				return false;
			Rpc::Globals::StringArgument argument;
			strcpy_s(argument.Argument, arg.c_str());
			NetCommandArgPtr ptr(Rpc::Globals::SerializeToCommand(&argument));

			//�������ƣ���Ϊ�㲥��ʽ��ͬ
			strcpy(ptr->user_token, type.c_str());
			strcpy(ptr->cmd_identity, "$***$");
			//�㲥
			int result = zmq_send(pub->second->_innerSocket, ptr.m_buffer, ptr.m_len, ZMQ_DONTWAIT);
			if (result <= 0)
			{
				check_zmq_error(pub->second->_zmq_state, pub->second->_innerAddress.c_str(), "�����㲥");
				return false;
			}
			log_debug1(DEBUG_REQUEST, 6, "�����㲥(%s)�ɹ�", pub->second->_outAddress);
			return true;
		}


		bool BroadcastingStation::poll()
		{
			const char* publish_address = _innerAddress.c_str();
			const char* request_address = _outAddress.c_str();
			_outSocket = create_socket(ZMQ_REP, request_address);
			_innerSocket = create_socket(ZMQ_PUB, publish_address);
			//�Ǽ��߳̿�ʼ
			_zmq_state = 0;
			set_command_thread_start();
			_station_state = station_state::Run;
			log_msg3("%s(%s | %s)������", _station_name, request_address, publish_address);
			//RedisLiveScope redis_live_scope;
			while (can_do())
			{
				poll_check_pause();
				//������������
				zmq_msg_t msg_call;
				int result = zmq_msg_init(&msg_call);
				if (result != 0)
				{
					_zmq_state = 2;//������
					break;
				}
				result = zmq_msg_recv(&msg_call, _outSocket, 0);
				if (result <= 0)
				{
					zmq_msg_close(&msg_call);
					if (!check_zmq_error(_zmq_state, request_address, "�������"))
						break;
					continue;
				}
				size_t len = zmq_msg_size(&msg_call);
				log_debug2(DEBUG_REQUEST, 6, "��������(%s)�ɹ�:%d", request_address, len);
				NetCommandArgPtr ptr(result);
				memcpy(ptr.m_buffer, static_cast<PNetCommand>(zmq_msg_data(&msg_call)), len);
				zmq_msg_close(&msg_call);

				char response[1];
				if (!check_crc(ptr.m_command))//(ptr.m_buffer[0] != '$' || (len > sizeof(NetCommand) &&  ptr.m_buffer[len - 1] != OBJ_TYPE_END))
				{
					send_result("֪ͨ�ط�", '1', "���յ�δ������", request_address);
					continue;
				}
				log_debug4(DEBUG_REQUEST, 6, "(%s)���յ�����,�û�%s,����%d(%s)", request_address, ptr->user_token, ptr->cmd_id, ptr->cmd_identity);

				//�������ƣ���Ϊ�㲥��ʽ��ͬ
				char tmp[GUID_LEN];
				memcpy(tmp, ptr->cmd_identity, GUID_LEN);
				memcpy(ptr->cmd_identity, ptr->user_token, GUID_LEN);
				memcpy(ptr->user_token, tmp, GUID_LEN);
				//�㲥
				result = zmq_send(_innerSocket, ptr.m_buffer, len, ZMQ_DONTWAIT);
				if (result <= 0)
				{
					if (!check_zmq_error(_zmq_state, publish_address, "�����㲥"))
					{
						send_result("�㲥ʧ��", '2', "�޷������㲥", request_address);
						break;
					}
					send_result("�㲥ʧ��", '2', "�޷������㲥", request_address);
					continue;
				}
				log_debug1(DEBUG_REQUEST, 6, "�����㲥(%s)�ɹ�", request_address);
				send_result("�㲥�ɹ�", '0', "�㲥�ɹ�", request_address);
			}
			_station_state = station_state::Closing;
			zmq_unbind(_outSocket, request_address);
			zmq_close(_outSocket);
			_outSocket = nullptr;
			zmq_unbind(_innerSocket, publish_address);
			zmq_close(_innerSocket);
			_innerSocket = nullptr;
			//�Ǽ��̹߳ر�
			set_command_thread_end();
			_station_state = station_state::Closed;
			return _zmq_state == 2 && get_net_state() == NET_STATE_RUNING;
		}
	}
}