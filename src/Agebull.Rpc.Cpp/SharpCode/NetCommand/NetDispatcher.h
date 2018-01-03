#ifndef ZMQ_API_NET_DISPATCHER_H
#pragma once
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		 * @brief ��ʾһ���㲥վ��
		 */
		class NetDispatcher :NetStation
		{
			////������Ϣ����
			//std::queue<NetCommandArgPtr> _queue;
			////������Ϣ������
			//boost::interprocess::interprocess_semaphore _semaphore;

		public:
			NetDispatcher()
				:NetStation("SystemManage", STATION_TYPE_DISPATCHER, ZMQ_REP, -1, -1)
			{

			}
			virtual ~NetDispatcher() {}
		private:
			/**
			* @brief ��ǰ��ķ�����
			*/
			static NetDispatcher* example;

			/**
			* @brief ��ͣ
			*/
			bool pause(bool waiting) override
			{
				return false;
			}

			/**
			* @brief ����
			*/
			bool resume(bool waiting)override
			{
				return false;
			}

			/**
			* @brief ����
			*/
			bool close(bool waiting)override
			{
				return false;
			}
		public:
			/**
			*��Ϣ��
			*/
			static void start()
			{
				if (!StationWarehouse::join(example))
				{
					delete example;
					return;
				}
				if (example->_zmq_state == 0)
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_out_address, example->_inner_address);
				else
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_out_address, example->_inner_address);
				bool reStrart = example->poll();
				StationWarehouse::left(example);
				if (reStrart)
				{
					delete example;
					example = new NetDispatcher();
					example->_zmq_state = -1;
					boost::thread thrds_s(boost::bind(start));
				}
				else
				{
					log_msg3("%s(%s | %s)�ѹر�", example->_station_name, example->_out_address, example->_inner_address);
					delete example;
					example = nullptr;
				}
			}

			/**
			* ����һ���㲥�߳�
			*/
			static void run()
			{
				if (example != nullptr)
				{
					return;
				}
				example = new NetDispatcher();
				boost::thread thrds_s(boost::bind(start));
			}

			/**
			* @brief ��ʼִ��һ������
			*/
			void command_start(const  char* caller, vector<string> lines)
			{
				exec_command(lines[0].c_str(), lines[1].c_str());
			}
			/**
			* @brief ����ִ��һ������
			*/
			void command_end(const  char* caller, vector<string> lines)
			{
				//send_result(lines[0]);
			}
		private:

			/**
			* @brief ������
			*/
			virtual void response()override
			{

			}
			/**
			* @brief ��������
			*/
			virtual void request()override;

			/**
			* ��������Ӧ
			*/
			virtual void heartbeat()override
			{

			}
		public:
			/**
			* @brief վ�㰲װ
			*/
			static string install_station(int type, string stattion);
			/**
			* @brief ȡ������Ϣ
			*/
			static string host_info(string stattion);
			/**
			*  @brief ����վ��
			*/
			static string start_station(string stattion);
			/**
			* @brief ��ͣվ��
			*/
			static string pause_station(string stattion);
			/**
			* @brief ����վ��
			*/
			static string resume_station(string stattion);
			/**
			* @brief �ر�վ��
			*/
			static string close_station(string stattion);
			/**
			* @brief ִ������
			*/
			static string exec_command(const char* command, const  char* argument);

		};
	}
}
#endif