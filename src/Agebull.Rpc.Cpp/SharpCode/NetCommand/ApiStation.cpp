#include "stdafx.h"
#include "ApiStation.h"
namespace agebull
{
	namespace zmq_net
	{
		ApiStation::ApiStation(string name)
			: BalanceStation<ApiStation, string, STATION_TYPE_API>(name)
		{
		}

		/**
		 * ��Զ�̵��ý���ʱ�Ĵ���
		 */
		void ApiStation::request()
		{
			char* client_addr = s_recv(_out_socket, 0);
			recv_empty(_out_socket);
			char* request = s_recv(_out_socket);

			const char* work = _balance.get_host();
			job_start(work, client_addr, request);

			free(client_addr);
			free(request);
		}
		/**
		* @brief ������ʼ�����͵������ߣ�
		*/
		bool ApiStation::job_start(const char* work, const char* client_addr, const  char* request)
		{
			//·�ɵ�����һ����������
			if (work == nullptr)
			{
				return job_end(client_addr, "NoWorker");
			}
			_zmq_state = s_sendmore(_inner_socket, work);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_inner_socket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_inner_socket, client_addr);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_inner_socket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_send(_inner_socket, request);//��ʵ����
			return _zmq_state >= 0;
		}

		/**
		* @brief ��������(���͵�������)
		*/
		bool ApiStation::job_end(const char* client_addr, const  char* response)
		{
			_zmq_state=s_sendmore(_out_socket, client_addr);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_out_socket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_send(_out_socket, response);
			return _zmq_state >= 0;
		}

		/**
		* @brief ��ʼִ��һ������
		*/
		void ApiStation::command_start(const char* caller, vector< string> lines)
		{
			const char* worker = lines.size()==1 ? _balance.get_host() : lines[0].c_str();
			const char* request = lines[lines.size() == 1 ? 0 : 1].c_str();
			job_start(caller, worker, request);
		}
		/**
		* @brief ����ִ��һ������
		*/
		void ApiStation::command_end(const char* caller, vector< string> lines)
		{
			job_end(caller, lines[0].c_str());
		}
		/**
		 * ��������������ʱ�Ĵ���
		 */
		void ApiStation::response()
		{
			// ��worker�ĵ�ַ���
			char* worker_addr = s_recv(_inner_socket);
			recv_empty(_inner_socket);
			char* client_addr = s_recv(_inner_socket);
			recv_empty(_inner_socket);
			char* response = s_recv(_inner_socket);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(client_addr, "READY") == 0)
			{
				worker_join(worker_addr, response, true);
			}
			else
			{
				job_end(client_addr, response);
			}
			free(worker_addr);
			free(client_addr);
			free(response);
		}
	}
}