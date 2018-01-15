#ifndef ZMQ_API_NET_DISPATCHER_H
#pragma once
#include <stdinc.h>
#include "ZeroStation.h"
#include "StationWarehouse.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		 * @brief ��ʾһ���㲥վ��
		 */
		class NetDispatcher :ZeroStation
		{
			/**
			* @brief ����
			*/
			static NetDispatcher* instance;
		public:
			/**
			* @brief ����
			*/
			NetDispatcher()
				:ZeroStation("SystemManage", STATION_TYPE_DISPATCHER, ZMQ_ROUTER, -1, -1)
			{

			}
			/**
			* @brief ����
			*/
			~NetDispatcher() override {}
			/**
			* @brief ��ʼִ��
			*/
			static void start(void*);

			/**
			*  @brief ִ��
			*/
			static void run()
			{
				if (instance != nullptr)
				{
					return;
				}
				instance = new NetDispatcher();
				zmq_threadstart(start, nullptr);
				//boost::thread thrds_s(boost::bind(start));
			}
		private:

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
			/**
			* @brief ��������
			*/
			void request(ZMQ_HANDLE socket)override;
		public:
			/**
			* @brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector<sharp_char> lines) override;
			/**
			* @brief վ�㰲װ
			*/
			static string install_station(string type_name, string stattion);
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
			* @brief Զ�̵���
			*/
			static string call_station(const char* stattion, vector<sharp_char>& arguments);
			/**
			* @brief ִ������
			*/
			static string exec_command(const char* command, vector<sharp_char> arguments);
			/**
			* @brief ִ������
			*/
			static string exec_command(const char* command, const char* argument);

		};
	}
}
#endif