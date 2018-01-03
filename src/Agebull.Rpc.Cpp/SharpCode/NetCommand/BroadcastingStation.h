#ifndef ZMQ_API_BROADCASTING_STATION_H
#pragma once
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{
		struct PublishItem
		{
			string publiher;
			string title;
			string arg;
		};

		/**
		* @brief ��ʾһ���㲥վ��
		*/
		class BroadcastingStationBase :public NetStation
		{
			/**
			* @brief ״̬�ź���
			*/
			boost::interprocess::interprocess_semaphore _pub_semaphore;
			queue<PublishItem> items;
		public:
			BroadcastingStationBase(string name,int type)
				:NetStation(name, type, ZMQ_REQ, -1, -1)
				, _pub_semaphore(1)
			{

			}
			/**
			*��Ϣ��
			*/
			static void dopub(BroadcastingStationBase* netobj)
			{
				netobj->poll_pub();
			}
			/**
			* ����һ���㲥�߳�
			*/
			bool poll_pub();

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
			bool publish(string publiher, string title, string arg);

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
				BroadcastingStation* station = new BroadcastingStation(publish_name);
				boost::thread(boost::bind(start, shared_ptr<BroadcastingStation>(station)));
			}
			/**
			*��Ϣ��
			*/
			static void start(shared_ptr<BroadcastingStation> arg)
			{
				BroadcastingStation* station = arg.get();
				if (!StationWarehouse::join(station))
				{
					return;
				}
				if (station->_zmq_state == 0)
					log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);
				else
					log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);
				boost::thread(boost::bind(dopub, static_cast<BroadcastingStationBase*>(station)));
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
			static SystemMonitorStation* example;
		public:
			SystemMonitorStation()
				:BroadcastingStationBase("SystemMonitor", STATION_TYPE_MONITOR)
			{

			}
			virtual ~SystemMonitorStation() {}

			/**
			* ����һ���㲥�߳�
			*/
			static void run()
			{
				if (example != nullptr)
					return;
				example = new SystemMonitorStation();
				boost::thread(boost::bind(start));
			}
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
				boost::thread(boost::bind(dopub, static_cast<BroadcastingStationBase*>(example)));
				bool reStrart = example->poll();
				StationWarehouse::left(example);
				if (reStrart)
				{
					//delete example;
					example = new SystemMonitorStation();
					example->_zmq_state = -1;
					boost::thread(boost::bind(start));
				}
				else
				{
					log_msg3("%s(%s | %s)�ѹر�", example->_station_name, example->_out_address, example->_inner_address);
					//delete example;
					//example = nullptr;
				}
			}
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