#ifndef ZMQ_API_STATION_H
#pragma once
#include <stdinc.h>
#include <utility>
#include "ZeroStation.h"
#include "BalanceStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* \brief ���ؾ��⴦����
		*/
		class host_balance :private map<string, time_t>
		{
			/**
			* \brief ��ǰ�������±�
			*/
			size_t _index;
			boost::mutex _mutex;
			vector<string> list;
		public:
			host_balance()
				: _index(0)
			{

			}

			/**
			* \brief ���뼯Ⱥ
			*/
			void join(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				auto iter = find(host);
				if (iter == end())
				{
					insert(make_pair(host, time(nullptr) + 10LL));
					list.emplace_back(host);
				}
				else
				{
					iter->second = time(nullptr) + 10LL;
				}
			}

			/**
			* \brief �����������
			*/
			void succees(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				auto iter = find(host);
				if (iter == end())
				{
					insert(make_pair(host, time(nullptr) + 10LL));
					list.emplace_back(host);
				}
				else
				{
					iter->second = time(nullptr) + 10LL;
				}
			}

			/**
			* \brief ��������ʧ��
			*/
			void bad(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_(host);
			}

			/**
			* \brief �˳���Ⱥ
			*/
			void left(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_(host);
			}

			/**
			* \brief ȡһ�����õ�����
			*/
			const char* get_host()
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				return get_host_();
			}
		private:
			/**
			* \brief �˳���Ⱥ
			*/
			void left_(const string& host)
			{
				erase(host);
				auto iter = list.begin();
				while (iter != list.end())
				{
					if (*iter == host)
					{
						list.erase(iter);
						break;
					}
					++iter;
				}
			}
			/**
			* \brief ȡһ�����õ�����
			*/
			const char* get_host_()
			{
				if (list.empty())
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
					if (++_index == size())
						_index = 0;
					auto iter = find(list[_index]);
					if (iter == end())
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
		* \brief APIվ��
		*/
		class api_station :public balance_station<api_station, string, STATION_TYPE_API>
		{
			/**
			* \brief ������Ϣ���з�����
			*/
			host_balance _balance;
		public:
			/**
			* \brief ����
			*/
			api_station(string name)
				: balance_station<api_station, string, STATION_TYPE_API>(std::move(name))
			{
			}

			/**
			* \brief ����
			*/
			virtual ~api_station() = default;

			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				zmq_threadstart(launch, new api_station(std::move(name)));
			}

			/**
			* \brief ִ��
			*/
			static void launch(void* arg)
			{
				api_station* station = static_cast<api_station*>(arg);
				if (!station_warehouse::join(station))
				{
					return;
				}
				if (!station->do_initialize())
					return;

				bool reStrart = station->poll();

				station_warehouse::left(station);
				station->destruct();
				if (reStrart)
				{
					api_station* station2 = new api_station(station->_station_name);
					station2->_zmq_state = ZmqSocketState::Again;
					zmq_threadstart(launch, station2);
				}
				else
				{
					log_msg3("Station:%s(%d | %d) is closed", station->_station_name.c_str(), station->_out_port, station->_inner_port);
				}
				delete station;
			}
		private:
			/**
			* \brief �������ϵ���Ӧ
			*/
			void response() override;
			/**
			* \brief ���ü��ϵ���Ӧ
			*/
			void request(ZMQ_HANDLE socket) override;
			/**
			* \brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector<sharp_char> lines) override;

			/**
			* \brief �������ϵ���Ӧ
			*/
			string create_item(const char* addr, const char* value) override
			{
				return value;
			}

			/**
			* \brief ������ʼ�����͵������ߣ�
			*/
			bool job_start(vector<sharp_char>& list);
			/**
			* \brief ��������(���͵�������)
			*/
			bool job_end(vector<sharp_char>& list);

			/**
			* \brief ���������˳�
			*/
			void worker_left(const char* addr) override
			{
				_balance.left(addr);
				balance_station<api_station, string, STATION_TYPE_API>::worker_left(addr);
			}

			/**
			* \brief �����������
			*/
			virtual void worker_join(const char* addr, const char* value, bool ready = false) override;
		};
	}
}
#endif
