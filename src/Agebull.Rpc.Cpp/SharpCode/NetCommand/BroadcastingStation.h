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
				:NetStation(name, STATION_TYPE_PUBLISH)
			{

			}
			virtual ~BroadcastingStation() {}
		private:
			/**
			*��Ϣ��
			*/
			bool poll();
			/**
			* @brief ��ǰ��ķ�����
			*/
			static map<string, shared_ptr<BroadcastingStation>> examples;

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
			*��Ϣ��
			*/
			static bool publish(string name, string type, string arg);
			/**
			* @brief ���ùر�����
			*/
			static void close_all(bool waiting)
			{
				map<string, shared_ptr<BroadcastingStation>>::iterator end = examples.end();
				for_each(examples.begin(), examples.end(), [](pair<string, shared_ptr<BroadcastingStation>> iter)
				{
					iter.second->close(false);
				});
				if (waiting)
				{
					while (examples.size() > 0)
						thread_sleep(250);
				}
			}
		};
	}
}
