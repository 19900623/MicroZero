#ifndef ZMQ_API_NET_DISPATCHER_H
#pragma once
#include <stdinc.h>
#include "NetStation.h"
#include "StationWarehouse.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		 * @brief ��ʾһ���㲥վ��
		 */
		class NetDispatcher :NetStation
		{
			/**
			* @brief ��ǰ��ķ�����
			*/
			static NetDispatcher* example;
		public:
			NetDispatcher()
				:NetStation("SystemManage", STATION_TYPE_DISPATCHER, ZMQ_ROUTER, -1, -1)
			{

			}
			virtual ~NetDispatcher() {}
		//private:

		//	/**
		//	* @brief ��ͣ
		//	*/
		//	bool pause(bool waiting) override
		//	{
		//		return false;
		//	}

		//	/**
		//	* @brief ����
		//	*/
		//	bool resume(bool waiting)override
		//	{
		//		return false;
		//	}

		//	/**
		//	* @brief ����
		//	*/
		//	bool close(bool waiting)override
		//	{
		//		return false;
		//	}
		public:
			/**
			*��Ϣ��
			*/
			static void start(void*)
			{
				if (!StationWarehouse::join(example))
				{
					delete example;
					return;
				}
				if (example->_zmq_state == ZmqSocketState::Succeed)
					log_msg3("%s(%d | %d)��������", example->_station_name.c_str(), example->_out_port, example->_inner_port)
				else
					log_msg3("%s(%d | %d)��������", example->_station_name.c_str(), example->_out_port, example->_inner_port)
				if (!example->initialize())
				{
					log_msg3("%s(%d | %d)�޷�����", example->_station_name.c_str(), example->_out_port, example->_inner_port)
					return;
				}
				log_msg3("%s(%d | %d)��������", example->_station_name.c_str(), example->_out_port, example->_inner_port)
				bool reStrart = example->poll();
				StationWarehouse::left(example);
				example->destruct();
				if (reStrart)
				{
					delete example;
					example = new NetDispatcher();
					example->_zmq_state = ZmqSocketState::Again;
					zmq_threadstart(start, nullptr);
				}
				else
				{
					log_msg3("%s(%d | %d)�ѹر�", example->_station_name.c_str(), example->_out_port, example->_inner_port)
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
				zmq_threadstart(start, nullptr);
				//boost::thread thrds_s(boost::bind(start));
			}

			/**
			* @brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector<string> lines) override
			{
				string val = call_station(caller, lines[0].c_str(), lines[1].c_str());
				return sharp_char(val);
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
			virtual void request(ZMQ_HANDLE socket)override;

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
			* @brief Զ�̵���
			*/
			static string call_station(string stattion, string command, string argument);
			/**
			* @brief ִ������
			*/
			static string exec_command(const char* command, const  char* argument);

		};
	}
}
#endif