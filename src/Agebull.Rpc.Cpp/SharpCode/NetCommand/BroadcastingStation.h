#ifndef ZMQ_API_BROADCASTING_STATION_H
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
		class BroadcastingStation :NetStation
		{

		public:
			BroadcastingStation(string name)
				:NetStation(name, STATION_TYPE_PUBLISH, ZMQ_REQ, ZMQ_PUB, -1)
			{

			}
			virtual ~BroadcastingStation() {}
		public:
			/**
			*��Ϣ��
			*/
			static void start(shared_ptr<BroadcastingStation> netobj)
			{
				station_run(netobj);
			}

			/**
			* ����һ���㲥�߳�
			*/
			static void run(string publish_name)
			{
				BroadcastingStation* route = new BroadcastingStation(publish_name);
				boost::thread thrds_s1(boost::bind(start, shared_ptr<BroadcastingStation>(route)));
			}

			/**
			* @brief ��ʼִ��һ������
			*/
			void command_start(const char* caller, vector< string> lines) override
			{
				publish(caller, lines[0], lines[1]);
			}
			/**
			* @brief ����ִ��һ������
			*/
			void command_end(const char* caller, vector<string> lines) override
			{
				publish(caller, lines[0], lines[1]);
			}
			/**
			*@brief �㲥����
			*/
			static bool publish(string station, string publiher, string title, string arg);
			/**
			*@brief �㲥����
			*/
			static bool publish_monitor(string publiher, string title, string arg);
			/**
			*@brief �㲥����
			*/
			bool publish(string publiher, string title, string arg) const;

			/**
			* @brief ������
			*/
			void response()override;
			/**
			* @brief ��������
			*/
			void request()override;

			/**
			* ��������Ӧ
			*/
			void heartbeat()override {}
		};
	}
}
#endif