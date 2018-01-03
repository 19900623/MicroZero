/**
 * ZMQ�㲥������
 *
 *
 */

#include "stdafx.h"
#include "BroadcastingStation.h"

namespace agebull
{
	namespace zmq_net
	{
#define send_result(code,type,msg,addr)\
	log_error3("%s(%s),����%s",msg, addr,code);\
	response[0] = type;\
	result = zmq_send(_out_socket, response, 1, ZMQ_DONTWAIT);\
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
		bool BroadcastingStation::publish_monitor(string publiher, string title, string arg)
		{
			return publish("SystemMonitor",publiher, title, arg);
		}
		/**
		*��Ϣ��
		*/
		bool BroadcastingStation::publish(string station, string publiher, string title, string arg)
		{
			auto pub = StationWarehouse::find(station);
			if (pub == nullptr || pub->_station_type != STATION_TYPE_PUBLISH)
				return false;
			return static_cast<BroadcastingStation*>(pub)->publish(publiher, title, arg);
		}
		/**
		*��Ϣ��
		*/
		bool BroadcastingStation::publish(string publiher, string title, string arg) const
		{
			s_sendmore(_inner_socket, title.c_str());
			s_sendmore(_inner_socket, "");
			s_send(_inner_socket, publiher.c_str());
			s_sendmore(_inner_socket, "");
			s_send(_inner_socket, arg.c_str());
			return true;
		}

		/**
		* @brief ������
		*/
		void BroadcastingStation::response()
		{
			
		}
		/**
		* @brief ��������
		*/
		void BroadcastingStation::request()
		{
			char* client_addr = s_recv(_out_socket, 0);
			recv_empty(_out_socket);
			char* title = s_recv(_out_socket);
			recv_empty(_out_socket);
			char* content = s_recv(_out_socket);
			publish(client_addr, title, content);
		}

	}
}