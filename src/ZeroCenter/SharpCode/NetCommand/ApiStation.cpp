#include "stdafx.h"
#include "ApiStation.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		* \brief ��ʼִ��һ������
		*/
		sharp_char api_station::command(const char* caller, vector<sharp_char> lines)
		{
			vector<sharp_char> response;
			inproc_request_socket<ZMQ_REQ> socket(caller, _station_name.c_str());
			if(socket.request(lines, response))
				return response.empty() ? "error" : response[0];
			switch(socket.get_state())
			{
			case zmq_socket_state::TimedOut:
				return "time out";
			default:
				return "net error";
			}
		}
		/**
		 * ��Զ�̵��ý���ʱ�Ĵ���
		 */
		void api_station::request(ZMQ_HANDLE socket)
		{
			vector<sharp_char> list;
			//0 �����ַ 1 ��֡ 2���� 3�����ʶ 4������ 5����
			_zmq_state = recv(socket, list);
			if (_zmq_state == zmq_socket_state::TimedOut)
			{
				return;
			}
			if (_zmq_state != zmq_socket_state::Succeed)
			{
				log_error3("������Ϣʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}
			if (list[2][0] == '@')//�ƻ�����
			{
				save_plan(socket, list);
				_zmq_state = send_addr(socket, *list[0]);
				_zmq_state = send_late(socket, "plan");
			}
			else
			{
				job_start(list);
			}
		}
		/**
		* \brief ������ʼ�����͵������ߣ�
		*/
		bool api_station::job_start(vector<sharp_char>& list)
		{
			//0 �����ַ 1 ��֡ 2���� 3�����ʶ 4������ 5����
			if (list.empty())
			{
				return false;
			}
			const char* client_addr = *list[0];

			if(list.size() < 4 || list[2].empty())
			{
				const ZMQ_HANDLE socket = list[0][0] == '_' ? _out_socket_inproc : _out_socket;
				_zmq_state = send_addr(socket, client_addr);
				_zmq_state = send_late(socket, "-Invalid");
				return _zmq_state == zmq_socket_state::Succeed;
			}
			//����֪ͨ�����˳�(�а�ȫ���գ������ⲿ����)
			if (list[2][0] == '#' && list[0][0] == '_')
			{
				_zmq_state = send_addr(_inner_socket, *list[3]);
				_zmq_state = send_late(_inner_socket, "bye");
				return _zmq_state == zmq_socket_state::Succeed;
			}
			//·�ɵ�����һ����������
			const char* worker = _balance.get_host();
			if (worker == nullptr)
			{
				ZMQ_HANDLE socket = list[0][0] == '_' ? _out_socket_inproc : _out_socket;
				_zmq_state = send_addr(socket, client_addr);
				_zmq_state = send_late(socket, "-NoWork");
				return _zmq_state == zmq_socket_state::Succeed;
			}

			_zmq_state = send_addr(_inner_socket, worker);
			//list.erase(++list.begin());
			_zmq_state = send(_inner_socket, list);
			return _zmq_state == zmq_socket_state::Succeed;
		}

		/**
		* \brief ��������(���͵�������)
		*/
		bool api_station::job_end(vector<sharp_char>& list)
		{
			assert(list.size() >= 3 && !list[2].empty());
			
			//list.erase(b);
			list[2].swap(list[1]);
			_zmq_state = send(list[1][0] == '_' ? _out_socket_inproc : _out_socket, ++list.begin(), list.end());
			return _zmq_state == zmq_socket_state::Succeed;
		}

		void api_station::worker_join(const char* addr, const char* value, bool ready)
		{
			if (addr == nullptr || strlen(addr) == 0)
				return;
			if (ready)
			{
				_balance.join(addr);
				balance_station<api_station, string, STATION_TYPE_API>::worker_join(addr, value, ready);
			}
			else
			{
				_balance.succees(addr);
			}
		}

		/**
		 * ��������������ʱ�Ĵ���
		 */
		void api_station::response()
		{
			vector<sharp_char> list;
			//0 worker��ַ 1��֡ 2�����ߵ�ַ 3�����ʶ 4���ؽ��
			_zmq_state = recv(_inner_socket, list);
			if (_zmq_state == zmq_socket_state::TimedOut)
			{
				return;
			}
			if (_zmq_state != zmq_socket_state::Succeed)
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