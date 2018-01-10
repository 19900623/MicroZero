#ifndef ZMQ_API_STATION_H
#pragma once
#include <stdinc.h>
#include "ZeroStation.h"
#include "BalanceStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* @brief ���ؾ��⴦����
		*/
		class HostBalance :private map<string, time_t>
		{
			/**
			* @brief ��ǰ�������±�
			*/
			size_t _index;
			boost::mutex _mutex;
			vector<string> list;
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
				boost::lock_guard<boost::mutex> guard(_mutex);
				auto iter = find(host);
				if (iter == end())
				{
					insert(make_pair(host, time(nullptr) + 10LL));
					list.push_back(host);
				}
				else
				{
					iter->second = time(nullptr) + 10LL;
				}
			}

			/**
			* @brief �����������
			*/
			void succees(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				auto iter = find(host);
				if (iter == end())
				{
					insert(make_pair(host, time(nullptr) + 10LL));
					list.push_back(host);
				}
				else
				{
					iter->second = time(nullptr) + 10LL;
				}
			}

			/**
			* @brief ��������ʧ��
			*/
			void bad(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_(host);
			}

			/**
			* @brief �˳���Ⱥ
			*/
			void left(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_(host);
			}

			/**
			* @brief ȡһ�����õ�����
			*/
			const char* get_host()
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				return get_host_();
			}
		private:
			/**
			* @brief �˳���Ⱥ
			*/
			void left_(string host)
			{
				erase(host);
				auto iter = list.begin();
				while (iter != list.end())
				{
					if (iter->compare(host) == 0)
					{
						list.erase(iter);
						break;
					}
					++iter;
				}
			}
			/**
			* @brief ȡһ�����õ�����
			*/
			const char* get_host_()
			{
				if (size() == 0)
					return nullptr;
				if (size() == 1)
				{
					auto iter = begin();
					if (time(nullptr) < iter->second)
						return iter->first.c_str();
					clear();
					list.clear();
					return nullptr;
				}
				{
					if (_index == size())
						_index = 0;
					auto iter = find(list[_index]);
					if(iter == end())
					{
						left_(list[_index]);
						return get_host_();
					}
					if (time(nullptr) < iter->second)
						return iter->first.c_str();
					erase(iter);
				}
				return get_host_();
			}
		};

		/**
		* @brief APIվ��
		*/
		class ApiStation :public BalanceStation<ApiStation, string, STATION_TYPE_API>
		{
			HostBalance _balance;
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
			bool job_start(vector<sharp_char>& list);
			/**
			* @brief ��������(���͵�������)
			*/
			bool job_end(vector<sharp_char>& list);

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
				if (ready)
				{
					_balance.join(addr);
					BalanceStation<ApiStation, string, STATION_TYPE_API>::worker_join(addr, value, ready);
				}
				else
				{
					_balance.succees(addr);
				}
			}
		};
	}
}
#endif
