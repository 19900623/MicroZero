#ifndef ZMQ_API_BROADCASTING_STATION_H
#pragma once
#include <stdinc.h>
#include <utility>
#include "ZeroStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* \brief ��ʾһ���㲥վ��
		*/
		class broadcasting_station_base :public zero_station
		{
		public:
			broadcasting_station_base(string name, int type)
				: zero_station(std::move(name), type, ZMQ_ROUTER, ZMQ_PUB, -1)
			{
			}

			/**
			* \brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector< sharp_char> lines) override
			{
				bool res = publish(caller, lines[0], lines[1], lines[1]);
				return sharp_char(res ? ZERO_STATUS_OK : ZERO_STATUS_FAILED);
			}


			/**
			* \brief ��������
			*/
			void request(ZMQ_HANDLE socket, bool inner)override;

			/**
			* ��������Ӧ
			*/
			void heartbeat()override {}
			/**
			* \brief ������
			*/
			void response()override {}

			/**
			*\brief ������Ϣ
			*/
			bool publish(const sharp_char& title, const sharp_char& description, vector<sharp_char>& datas);

			/**
			*\brief ������Ϣ
			*/
			bool publish(const string& publiher, const string& title, const string& arg);
			/**
			*\brief �㲥����
			*/
			bool publish(const string& caller, const string& title, const string& sub, const string& arg);
			/**
			*\brief �㲥����
			*/
			bool publish(const string& caller, const string& title, const string& plan, const string& sub, const string& arg) const;
		
		};

		/**
		 * \brief ��ʾһ���㲥վ��
		 */
		class broadcasting_station :public broadcasting_station_base
		{
		public:
			broadcasting_station(string name)
				:broadcasting_station_base(name, STATION_TYPE_PUBLISH)
			{

			}
			virtual ~broadcasting_station() = default;
			/**
			* ����һ���㲥�߳�
			*/
			static void run(string publish_name)
			{
				zmq_threadstart(launch, new broadcasting_station(publish_name));
			}
			/**
			*��Ϣ��
			*/
			static void launch(void* arg);
			/**
			*\brief �㲥����
			*/
			static bool publish(string station, string publiher, string title, string sub, string arg);
		};


		/**
		* \brief ��ʾһ���㲥վ��
		*/
		class system_monitor_station :broadcasting_station_base
		{
			/**
			* \brief �ܷ��������
			*/
			bool can_do() const override
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) && get_net_state() < NET_STATE_DISTORY;
			}
			static system_monitor_station* example_;
		public:
			system_monitor_station()
				:broadcasting_station_base("SystemMonitor", STATION_TYPE_MONITOR)
			{

			}
			virtual ~system_monitor_station() = default;

			///**
			//* \brief ��ͣ
			//*/
			//bool pause(bool waiting) override
			//{
			//	return false;
			//}

			///**
			//* \brief ����
			//*/
			//bool resume(bool waiting)override
			//{
			//	return false;
			//}

			///**
			//* \brief ����
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
				if (example_ != nullptr)
					return;
				zmq_threadstart(launch, nullptr);
			}
			/**
			*��Ϣ��
			*/
			static void launch(void*);

			/**
			*\brief �㲥����
			*/
			static bool monitor(string publiher, string state, string content)
			{
				if (example_ != nullptr)
					return example_->publish(publiher, state, content);
				return false;
			}
		};
	}
}
#endif