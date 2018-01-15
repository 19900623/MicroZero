#include "stdafx.h"
#include "ApiStation.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		* @brief ��ʼִ��һ������
		*/
		sharp_char ApiStation::command(const char* caller, vector<sharp_char> lines)
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
			//0 �����ַ 1 ��֡ 2���� 3�����ʶ 4������ 5����
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
			job_start(list);
		}
		/**
		* @brief ������ʼ�����͵������ߣ�
		*/
		bool ApiStation::job_start(vector<sharp_char>& list)
		{
			//0 �����ַ 1 ��֡ 2���� 3�����ʶ 4������ 5����
			if (list.empty())
			{
				return false;
			}
			const char* client_addr = *list[0];
			if(list.size() < 4 || list[2].size() == 0)
			{
				ZMQ_HANDLE socket = list[0][0] == '_' ? _out_socket_inproc : _out_socket;
				_zmq_state = send_addr(socket, client_addr);
				_zmq_state = send_late(socket, "Invalid");
				return _zmq_state == ZmqSocketState::Succeed;
			}
			//����֪ͨ�����˳�(�а�ȫ���գ������ⲿ����)
			if (list[2][0] == '#' && list[0][0] == '_')
			{
				_zmq_state = send_addr(_inner_socket, *list[3]);
				_zmq_state = send_late(_inner_socket, "bye");
				return _zmq_state == ZmqSocketState::Succeed;
			}
			//·�ɵ�����һ����������
			const char* worker = _balance.get_host();
			if (worker == nullptr)
			{
				ZMQ_HANDLE socket = list[0][0] == '_' ? _out_socket_inproc : _out_socket;
				_zmq_state = send_addr(socket, client_addr);
				_zmq_state = send_late(socket, "NoWork");
				return _zmq_state == ZmqSocketState::Succeed;
			}

			_zmq_state = send_addr(_inner_socket, worker);
			//list.erase(++list.begin());
			_zmq_state = send(_inner_socket, list);
			return _zmq_state == ZmqSocketState::Succeed;
		}

		/**
		* @brief ��������(���͵�������)
		*/
		bool ApiStation::job_end(vector<sharp_char>& list)
		{
			assert(list.size() >= 3 && list[2].size() > 0);
			
			//list.erase(b);
			list[2].swap(list[1]);
			_zmq_state = send(list[1][0] == '_' ? _out_socket_inproc : _out_socket, ++list.begin(), list.end());
			return _zmq_state == ZmqSocketState::Succeed;
		}

		/**
		 * ��������������ʱ�Ĵ���
		 */
		void ApiStation::response()
		{
			vector<sharp_char> list;
			//0 worker��ַ 1��֡ 2�����ߵ�ַ 3�����ʶ 4���ؽ��
			_zmq_state = recv(_inner_socket, list);
			if (_zmq_state == ZmqSocketState::TimedOut)
			{
				return;
			}
			if (_zmq_state != ZmqSocketState::Succeed)
			{
				log_error3("���ս��ʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}
			switch (list[2][0])
			{
			case '@'://����
				worker_join(*list[0], *list[3], true);
				send_addr(_inner_socket, *list[0]);
				send_late(_inner_socket, "wecome");
				return;
			case '*'://��ʼ����
				return;
			default:
				break;
			}
			job_end(list);
		}
	}
}