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
				:NetStation(name, STATION_TYPE_DISPATCHER)
			{

			}
			virtual ~NetDispatcher() {}
		private:
			/**
			*��Ϣ��
			*/
			bool poll();
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
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_outAddress, example->_innerAddress);
				else
					log_msg3("%s(%s | %s)��������", example->_station_name, example->_outAddress, example->_innerAddress);
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
					log_msg3("%s(%s | %s)�ѹر�", example->_station_name, example->_outAddress, example->_innerAddress);
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
			* @brief �������ϵ���Ӧ
			*/
			void onCallerPollIn();

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
