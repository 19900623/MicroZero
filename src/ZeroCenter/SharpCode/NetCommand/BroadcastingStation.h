#ifndef ZMQ_API_BROADCASTING_STATION_H
#pragma once
#include <stdinc.h>
#include "ZeroStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* @brief ��ʾһ���㲥վ��
		*/
		class BroadcastingStationBase :public ZeroStation
		{
		public:
			BroadcastingStationBase(string name, int type)
				: ZeroStation(name, type, ZMQ_ROUTER, ZMQ_PUB, -1)
			{
			}

			/**
			* @brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector< sharp_char> lines) override
			{
				bool res = publish(caller, lines[0], lines[1]);
				return sharp_char(res ? "OK" : "Bad");
			}

			/**
			*@brief �㲥����
			*/
			bool publish(string caller, string title, string arg) const;
			/**
			*@brief �㲥����
			*/
			bool publish(string caller, string title, string plan, string arg) const;

			/**
			* @brief ��������
			*/
			void request(ZMQ_HANDLE socket)override;

			/**
			* ��������Ӧ
			*/
			void heartbeat()override {}
			/**
			* @brief ������
			*/
			void response()override {}

			/**
			*@brief ������Ϣ
			*/
			bool pub_data(string publiher, string title, string arg);

			/**
			*@brief ������Ϣ
			*/
			bool pub_data(string publiher, string line);
		};

		/**
		 * @brief ��ʾһ���㲥վ��
		 */
		class BroadcastingStation :public BroadcastingStationBase
		{
		public:
			BroadcastingStation(string name)
				:BroadcastingStationBase(name, STATION_TYPE_PUBLISH)
			{

			}
			virtual ~BroadcastingStation() {}
			/**
			* ����һ���㲥�߳�
			*/
			static void run(string publish_name)
			{
				zmq_threadstart(start, new BroadcastingStation(publish_name));
			}
			/**
			*��Ϣ��
			*/
			static void start(void* arg);
			/**
			*@brief �㲥����
			*/
			static bool publish(string station, string publiher, string title, string arg);
		};


		/**
		* @brief ��ʾһ���㲥վ��
		*/
		class SystemMonitorStation :BroadcastingStationBase
		{
			/**
			* @brief �ܷ��������
			*/
			bool can_do() const override
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) && get_net_state() < NET_STATE_DISTORY;
			}
			static SystemMonitorStation* example;
		public:
			SystemMonitorStation()
				:BroadcastingStationBase("SystemMonitor", STATION_TYPE_MONITOR)
			{

			}
			virtual ~SystemMonitorStation() {}

			///**
			//* @brief ��ͣ
			//*/
			//bool pause(bool waiting) override
			//{
			//	return false;
			//}

			///**
			//* @brief ����
			//*/
			//bool resume(bool waiting)override
			//{
			//	return false;
			//}

			///**
			//* @brief ����
			//*/
			//bool close(bool waiting)override
			//{
			//	return false;
			//}
			/**
			* ����һ���㲥�߳�
			*/
			static void run()
			{
				if (example != nullptr)
					return;
				zmq_threadstart(start, nullptr);
			}
			/**
			*��Ϣ��
			*/
			static void start(void*);

			/**
			*@brief �㲥����
			*/
			static bool monitor(string publiher, string state, string content)
			{
				if (example != nullptr)
					return example->publish(publiher, state, content);
				return false;
			}

		};
	}
}
#endif