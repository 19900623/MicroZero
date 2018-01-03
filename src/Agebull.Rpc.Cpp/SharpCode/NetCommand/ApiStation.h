#ifndef ZMQ_API_STATION_H
#pragma once
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{

		/**
		* @brief ���ؾ��⴦����
		*/
		class HostBalance :public vector<string>
		{
			/**
			* @brief ��ǰ�������±�
			*/
			size_t _index;
		public:
			HostBalance()
				:vector<string>(16)
				, _index(0)
			{

			}
			/**
			* @brief ���뼯Ⱥ
			*/
			void join(const char* host)
			{
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
				size_t s = size();
				if (s == 0)
					return nullptr;
				if (s == 1)
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
		public:
			/**
			* @brief ����
			*/
			ApiStation(string name);
			/**
			* @brief ����
			*/
			virtual ~ApiStation() {}
			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				ApiStation* netobj = new ApiStation(name);
				boost::thread thrds_s1(boost::bind(start, shared_ptr<ApiStation>(netobj)));
			}
			/**
			* @brief ִ��
			*/
			static void start(shared_ptr<ApiStation> arg)
			{
				ApiStation* station = arg.get();
				if (!StationWarehouse::join(station))
				{
					return;
				}
				if (station->_zmq_state == 0)
					log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);
				else
					log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);
				bool reStrart = station->poll();
				StationWarehouse::left(station);
				if (reStrart)
				{
					run(station->_station_name);
				}
				else
				{
					log_msg3("%s(%s | %s)�ѹر�", station->_station_name, station->_out_address, station->_inner_address);
				}
			}
		private:
			/**
			* @brief �������ϵ���Ӧ
			*/
			void response() override;
			/**
			* @brief ���ü��ϵ���Ӧ
			*/
			void request() override;
			/**
			* @brief ��ʼִ��һ������
			*/
			void command_start(const char* caller, vector< string> lines) override;
			/**
			* @brief ����ִ��һ������
			*/
			void command_end(const char* caller, vector< string> lines)override;
			/**
			* @brief �������ϵ���Ӧ
			*/
			string create_item(const char* addr, const char * value)override
			{
				return value;
			}
			/**
			* @brief ������ʼ�����͵������ߣ�
			*/
			bool job_start(const char* work, const char* client_addr, const  char* request);
			/**
			* @brief ��������(���͵�������)
			*/
			bool job_end(const char* client_addr, const  char* response);

			/**
			* @brief ���������˳�
			*/
			void worker_left(char* addr)override
			{
				_balance.left(addr);
				BalanceStation<ApiStation, string, STATION_TYPE_API>::worker_left(addr);
			}

			/**
			* @brief �����������
			*/
			void worker_join(char* addr, char* value, bool ready = false)override
			{
				_balance.join(addr);
				BalanceStation<ApiStation, string, STATION_TYPE_API>::worker_join(addr, value, ready);
			}
		};
	}
}
#endif