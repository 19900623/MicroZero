#include "stdafx.h"
#include "ApiStation.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		* @brief ��ʼִ��һ������
		*/
		sharp_char ApiStation::command(const char* caller, vector<string> lines)
		{
			vector<sharp_char> response;
			RequestSocket<ZMQ_REQ,false> socket(caller, _station_name.c_str());
			if(socket.request(lines, response))
				return response.size() == 0 ? "error" : response[0];
			switch(socket.get_state())
			{
			case ZmqSocketState::TimedOut:
				return "time out";
			default:
				return "net error";
			}
		}
		/**
		 * ��Զ�̵��ý���ʱ�Ĵ���
		 */
		void ApiStation::request(ZMQ_HANDLE socket)
		{
			vector<sharp_char> list;
			//0 �����ַ 1 ��֡ 2���� 3����
			_zmq_state = recv(socket, list);
			if (_zmq_state == ZmqSocketState::TimedOut)
			{
				return;
			}
			if (_zmq_state != ZmqSocketState::Succeed)
			{
				log_error3("������Ϣʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}

			//cout << "request:" << ((boost::posix_time::microsec_clock::universal_time() - _start).total_microseconds() /*/ 1000.0*/) << "ms" << endl;
			//_start = boost::posix_time::microsec_clock::universal_time();
			if (_zmq_state == ZmqSocketState::Empty)
			{
				job_end(*list[0], "EmptyRequest");
				return;
			}
			job_start(*(list[0]), *(list[2]), *(list[3]));
		}
		/**
		* @brief ������ʼ�����͵������ߣ�
		*/
		bool ApiStation::job_start(const char* client_addr, const char* command, const  char* request)
		{
			//·�ɵ�����һ����������
			const char* worker = _balance.get_host();
			if (worker == nullptr)
			{
				return job_end(client_addr, "NoWork");
			}
			_zmq_state = send_addr(_inner_socket, worker);
			_zmq_state = send_more(_inner_socket, client_addr);
			_zmq_state = send_more(_inner_socket, command);
			_zmq_state = send_late(_inner_socket, request);
			return _zmq_state == ZmqSocketState::Succeed;
		}

		/**
		* @brief ��������(���͵�������)
		*/
		bool ApiStation::job_end(const char* client_addr, const char* response)
		{
			if (client_addr == nullptr)
				return true;
			ZMQ_HANDLE socket = client_addr[0] == '_' ? _out_socket_inproc : _out_socket;
			_start = boost::posix_time::microsec_clock::universal_time();
			_zmq_state = send_addr(socket, client_addr);
			_zmq_state = send_late(socket, response);
			//cout << "route:" << ((boost::posix_time::microsec_clock::universal_time() - _start).total_microseconds() /*/ 1000.0*/) << "ms" << endl;
			//_start = boost::posix_time::microsec_clock::universal_time();

			return _zmq_state == ZmqSocketState::Succeed;
		}

		/**
		 * ��������������ʱ�Ĵ���
		 */
		void ApiStation::response()
		{
			vector<sharp_char> list;
			//0 worker��ַ 1��֡ 2�����ߵ�ַ 3�����ʶ 4���ؽ��
			_zmq_state = recv(_out_socket, list);
			if (_zmq_state == ZmqSocketState::TimedOut)
			{
				return;
			}
			if (_zmq_state != ZmqSocketState::Succeed)
			{
				log_error3("���ս��ʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}
			if (strcmp(*list[2], "@") == 0)
			{
				worker_join(*list[0], *list[3], true);
			}
			else
			{
				job_end(*list[2], *list[3]);
			}
		}
	}
}