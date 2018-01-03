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
			NetDispatcher(string name)
				:NetStation(name, STATION_TYPE_DISPATCHER, ZMQ_REQ, -1, -1)
			{

			}
			virtual ~NetDispatcher() {}
		private:
			/**
			* @brief ��ǰ��ķ�����
			*/
			static NetDispatcher* example;

		public:
			/**
			*��Ϣ��
			*/
			static void start()
			{
				StationWarehouse::join(example);
				if (example->_zmq_state == 0)
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_out_address, example->_inner_address);
				else
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_out_address, example->_inner_address);
				bool reStrart = example->poll();
				StationWarehouse::left(example);
				if (reStrart)
				{
					example = new NetDispatcher(example->_station_name);
					example->_zmq_state = -1;
					boost::thread thrds_s(boost::bind(start));
				}
				else
				{
					log_msg3("%s(%s | %s)�ѹر�", example->_station_name, example->_out_address, example->_inner_address);
				}
			}

			/**
			* ����һ���㲥�߳�
			*/
			static void run(const char* name)
			{
				if (example != nullptr)
				{
					return;
				}
				example = new NetDispatcher(name);
				boost::thread thrds_s(boost::bind(start));
			}

			/**
			*��Ϣ��
			*/
			static bool send_result(string caller, string state);

			/**
			* @brief ��ʼִ��һ������
			*/
			void command_start(const char* caller, vector< string> lines)
			{
				exec_command(caller, lines[0].c_str(), lines[1].c_str());
			}
			/**
			* @brief ����ִ��һ������
			*/
			void command_end(const char* caller, vector< string> lines)
			{
				send_result(caller, lines[0]);
			}
		private:

			/**
			* @brief ������
			*/
			virtual void response()
			{

			}
			/**
			* @brief ��������
			*/
			virtual void request()override;

			/**
			* ��������Ӧ
			*/
			virtual void heartbeat()
			{

			}
		public:
			/**
			* @brief ��ͣվ��
			*/
			static void pause_station(string caller, string stattion);
			/**
			* @brief ����վ��
			*/
			static void resume_station(string caller, string stattion);
			/**
			*  @brief ����վ��
			*/
			static void start_station(string caller, string stattion);
			/**
			* @brief �ر�վ��
			*/
			static void close_station(string caller, string stattion);
			/**
			* @brief ִ������
			*/
			static void exec_command(const char* client_addr, const  char* command, const  char* argument);
			/**
			* @brief �ر�����վ��
			*/
			static void shutdown(string caller, string stattion);
		};
	}
}
#endif