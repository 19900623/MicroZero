#include "stdafx.h"
#include "ApiStation.h"
namespace agebull
{
	namespace zmq_net
	{
		ApiStation::ApiStation(string name)
			: RouteStation<ApiStation, string, STATION_TYPE_API>(name)
			, _nowWorkIndex(0)
		{
		}

		/**
		 *ȡ��һ�����õĹ���������
		 *
		 LRU�߼�
		 -һֱ��_innerSocket�л�ȡ��Ϣ�����г���һ��worker����ʱ�Ŵ�_outSocket��ȡ��Ϣ��
		 - ��woker��Ӧʱ���Ὣ��worker���Ϊ��׼���ã���ת��woker�Ļ�Ӧ��client
		 - ���client���������󣬾ͽ�������ת������һ��worker
		 */
		char* ApiStation::getNextWorker()
		{
			if (_workers.size() == 0)
				return nullptr;
			if (_workers.size() == 1)
				return _strdup(_hosts[0].c_str());
			while (_hosts.size() == 0)
			{
				thread_sleep(1);
			}
			auto host = _hosts.back();
			_hosts.pop_back();
			return _strdup(host.c_str());
		}
		/**
		 * ��Զ�̵��ý���ʱ�Ĵ���
		 */
		void ApiStation::onCallerPollIn()
		{
			// ��ȡ��һ��client�����󣬽������е�worker����
			// client�������Ϣ��ʽ�ǣ�[client��ַ][��֡][��������]
			char* client_addr = s_recv(_outSocket, 0);
			recv_empty(_outSocket);
			char* request = s_recv(_outSocket);

			while (true)
			{
				char* work = getNextWorker();
				if (snedToWorker(work, client_addr, request))
					break;
				left(work);
			}
			free(client_addr);
			free(request);
		}
		bool ApiStation::snedToWorker(char* work, char* client_addr, char* request)
		{
			//·�ɵ�����һ����������
			if (work == nullptr)
			{
				s_sendmore(_outSocket, client_addr);
				s_sendmore(_outSocket, "");
				s_send(_outSocket, "NoWorker");
				return true;
			}
			else
			{
				int state = s_sendmore(_innerSocket, work);
				if (state < 0)
					return false;
				state = s_sendmore(_innerSocket, "");
				if (state < 0)
					return false;
				state = s_sendmore(_innerSocket, client_addr);
				if (state < 0)
					return false;
				state = s_sendmore(_innerSocket, "");
				if (state < 0)
					return false;
				state = s_send(_innerSocket, request);//��ʵ����
				return state > 0;
			}
		}
		/**
		 * ��������������ʱ�Ĵ���
		 */
		void ApiStation::onWorkerPollIn()
		{
			// ��worker�ĵ�ַ���
			char* worker_addr = s_recv(_innerSocket);
			recv_empty(_innerSocket);
			char* client_addr = s_recv(_innerSocket);
			recv_empty(_innerSocket);
			char* reply = s_recv(_innerSocket);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(client_addr, "READY") == 0)
			{
				_hosts.push_back(worker_addr);
				join(worker_addr, reply, true);
			}
			else
			{
				s_sendmore(_outSocket, client_addr);
				s_sendmore(_outSocket, "");
				s_send(_outSocket, reply);//��ʵ����
				_hosts.insert(_hosts.begin(), worker_addr);
			}
			free(worker_addr);
			free(client_addr);
			free(reply);
		}

		/**
		* ��������������ʱ�Ĵ���
		*/
		void ApiStation::left(char* name)
		{
			auto iter = _hosts.begin();
			while (iter != _hosts.end())
			{
				if (*iter != name)
				{
					++iter;
					continue;
				}
				_hosts.erase(iter);
				break;
			}
			__super::left(name);
		}
	}
}