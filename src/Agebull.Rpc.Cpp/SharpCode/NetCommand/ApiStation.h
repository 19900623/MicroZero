#ifndef ZMQ_API_STATION_H
#pragma once
#include <stdinc.h>
#include "NetStation.h"
#include "BalanceStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* @brief ���ؾ��⴦����
		*/
		class HostBalance :private vector<string>
		{
			/**
			* @brief ��ǰ�������±�
			*/
			size_t _index;
		public:
			HostBalance()
				: _index(0)
			{
			}

			/**
			* @brief ���뼯Ⱥ
			*/
			void join(const char* host)
			{
				if (host == nullptr || strlen(host) == 0)
					return;
				push_back(host);
			}

			/**
			* @brief �����������
			*/
			void succees(const char* host)
			{
				//_hosts.push_back(host);
			}

			/**
			* @brief ��������ʧ��
			*/
			void bad(const char* host)
			{
				left(host);
			}

			/**
			* @brief �˳���Ⱥ
			*/
			void left(const char* host)
			{
				auto iter = begin();
				while (iter != end())
				{
					if (iter->compare(host) == 0)
					{
						erase(iter);
						break;
					}
					++iter;
				}
			}

			/**
			* @brief ȡһ�����õ�����
			*/
			const char* get_host()
			{
				if (size() == 0)
					return nullptr;
				if (size() == 1)
					return at(0).c_str();
				if (_index < size())
					return at(_index++).c_str();
				_index = 1;
				return at(0).c_str();
			}
		};

		/**
		* @brief APIվ��
		*/
		class ApiStation :public BalanceStation<ApiStation, string, STATION_TYPE_API>
		{
			HostBalance _balance;
			boost::posix_time::ptime  _start;
			/**
			* @brief ������Ϣ���з�����
			*/
			boost::mutex _mutex;
		public:
			/**
			* @brief ����
			*/
			ApiStation(string name)
				: BalanceStation<ApiStation, string, STATION_TYPE_API>(name)
			{
			}

			/**
			* @brief ����
			*/
			virtual ~ApiStation()
			{
			}

			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				zmq_threadstart(start, new ApiStation(name));
			}

			/**
			* @brief ִ��
			*/
			static void start(void* arg)
			{
				ApiStation* station = static_cast<ApiStation*>(arg);
				if (!StationWarehouse::join(station))
				{
					return;
				}
				if (station->_zmq_state == ZmqSocketState::Succeed)
					log_msg3("%s(%d | %d)��������", station->_station_name.c_str(), station->_out_port, station->_inner_port)
				else
					log_msg3("%s(%d | %d)��������", station->_station_name.c_str(), station->_out_port, station->_inner_port)
				if (!station->initialize())
				{
					log_msg3("%s(%d | %d)�޷�����", station->_station_name.c_str(), station->_out_port, station->_inner_port)
					return;
				}
				log_msg3("%s(%d | %d)��������", station->_station_name.c_str(), station->_out_port, station->_inner_port)
				bool reStrart = station->poll();

				StationWarehouse::left(station);
				station->destruct();
				if (reStrart)
				{
					ApiStation* station2 = new ApiStation(station->_station_name);
					station2->_zmq_state = ZmqSocketState::Again;
					zmq_threadstart(start, station2);
				}
				else
				{
					log_msg3("%s(%d | %d)�ѹر�", station->_station_name.c_str(), station->_out_port, station->_inner_port)
				}
				delete station;
			}
		private:
			/**
			* @brief �������ϵ���Ӧ
			*/
			void response() override;
			/**
			* @brief ���ü��ϵ���Ӧ
			*/
			void request(ZMQ_HANDLE socket) override;
			/**
			* @brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector< string> lines) override;

			/**
			* @brief �������ϵ���Ӧ
			*/
			string create_item(const char* addr, const char* value) override
			{
				return value;
			}

			/**
			* @brief ������ʼ�����͵������ߣ�
			*/
			bool job_start(const char* client_addr, const char* command, const char* request);
			/**
			* @brief ��������(���͵�������)
			*/
			bool job_end(const char* client_addr, const char* response);

			/**
			* @brief ���������˳�
			*/
			void worker_left(const char* addr) override
			{
				_balance.left(addr);
				BalanceStation<ApiStation, string, STATION_TYPE_API>::worker_left(addr);
			}

			/**
			* @brief �����������
			*/
			void worker_join(const char* addr, const char* value, bool ready = false) override
			{
				if (addr == nullptr || strlen(addr) == 0)
					return;
				_balance.join(addr);
				BalanceStation<ApiStation, string, STATION_TYPE_API>::worker_join(addr, value, ready);
			}
		};
	}
}
#endif
