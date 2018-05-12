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
			size_t index_;
			/**
			 * \brief ������
			 */
			boost::mutex _mutex;
			vector<const char*> list;
		public:
			host_balance()
				: index_(0)
			{

			}

			/**
			* \brief ���뼯Ⱥ
			*/
			void join(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				const iterator iter = find(host);
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
			* \brief �����������
			*/
			void succees(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				const iterator iter = find(host);
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
			* \brief ��������ʧ��
			*/
			void bad(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_inner(host);
			}

			/**
			* \brief �˳���Ⱥ
			*/
			void left(const char* host)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				left_inner(host);
			}

			/**
			* \brief ȡһ�����õ�����
			*/
			bool get_host(const char*& worker)
			{
				boost::lock_guard<boost::mutex> guard(_mutex);
				return get_next_host(worker);
			}
		private:
			/**
			* \brief �˳���Ⱥ
			*/
			void left_inner(const char* host)
			{
				erase(host);
				for (auto i = list.begin();i<list.end();++i)
				{
					if (_stricmp(*i,host) == 0)
					{
						list.erase(i);
						break;
					}
				}
			}
			/**
			* \brief ȡһ�����õ�����
			*/
			bool get_next_host(const char*& worker)
			{
				if (list.empty())
				{
					worker = nullptr;
					return true;
				}
				if (size() == 1)
				{
					const auto iter = begin();
					if (time(nullptr) < iter->second)
					{
						worker = iter->first.c_str();
						return true;
					}
					clear();
					list.clear();
					worker = nullptr;
					return true;
				}
				if (++index_ == size())
					index_ = 0;
				const auto iter = find(list[index_]);
				if (iter == end())
				{
					auto i = list.begin();
					i += index_;
					list.erase(i);
					return get_next_host(worker);
				}
				worker = list[index_];
				return time(nullptr) < iter->second;
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
				auto station = static_cast<api_station*>(arg);
				if (!station_warehouse::join(station))
				{
					return;
				}
				if (!station->do_initialize())
					return;
				zmq_threadstart(plan_poll, arg);
				const bool re_strart = station->poll();

				station_warehouse::left(station);
				station->destruct();
				if (re_strart)
				{
					api_station* station2 = new api_station(station->station_name_);
					station2->zmq_state_ = zmq_socket_state::Again;
					zmq_threadstart(launch, station2);
				}
				else
				{
					log_msg3("Station:%s(%d | %d) is closed", station->station_name_.c_str(), station->request_port_, station->response_port_);
				}
				sleep(1);
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
			void request(ZMQ_HANDLE socket, bool inner) override;
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
			* \brief ��������ƻ�
			*/
			bool job_plan(ZMQ_HANDLE socket, vector<sharp_char>& list);
			/**
			* \brief ������ʼ�����͵������ߣ�
			*/
			bool job_start(ZMQ_HANDLE socket, vector<sharp_char>& list);
			/**
			* \brief ��������(���͵�������)
			*/
			bool job_end(vector<sharp_char>& list);

			/**
			* \brief ���������˳�
			*/
			bool worker_left(const char* addr) override
			{
				if (addr == nullptr || strlen(addr) == 0)
					return false;
				if (!balance_station<api_station, string, STATION_TYPE_API>::worker_left(addr))
					return false;
				_balance.left(addr);
				return true;
			}

			/**
			* \brief �����������
			*/
			bool worker_join(const char* addr) override
			{
				if (addr == nullptr || strlen(addr) == 0)
					return false;
				if (!balance_station<api_station, string, STATION_TYPE_API>::worker_join(addr))
					return false;
				_balance.join(addr);
				return true;
			}
			/**
			* \brief �����������
			*/
			bool worker_heat(const char* addr) override
			{
				if (!balance_station<api_station, string, STATION_TYPE_API>::worker_heat(addr))
					return false;
				_balance.succees(addr);
				return true;
			}
		};
	}
}
#endif
