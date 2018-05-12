#ifndef ZMQ_API_BROADCASTING_STATION_H
#pragma once
#include "../stdinc.h"
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
				const bool res = publish(caller, lines[0], lines[1], lines[1]);
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
			bool publish(const string& publiher, const string& title, const string& sub, const string& arg);
			/**
			*\brief �㲥����
			*/
			bool publish(const string& publiher, const string& title, const string& sub, const string& plan, const string& arg) const;
		
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
			* \brief ����һ���㲥�߳�
			*/
			static void run(string publish_name)
			{
				zmq_threadstart(launch, new broadcasting_station(publish_name));
			}
			/**
			*\brief ����һ���㲥�߳�
			*/
			static void launch(void* arg);
			/**
			*\brief �㲥����
			*/
			static bool publish(const string& station, const string& publiher, const string& title, const string& sub, const string& arg);
		};


		/**
		* \brief ϵͳ����վ��
		*/
		class system_monitor_station :broadcasting_station_base
		{
			/**
			* \brief �ܷ��������
			*/
			bool can_do() const override
			{
				return (station_state_ == station_state::Run || station_state_ == station_state::Pause) && get_net_state() < NET_STATE_DISTORY;
			}

			/**
			* \brief ����
			*/
			static system_monitor_station* example_;
		public:
			system_monitor_station()
				:broadcasting_station_base("SystemMonitor", STATION_TYPE_MONITOR)
			{

			}
			virtual ~system_monitor_station() = default;

			/**
			* \brief ����һ���㲥�߳�
			*/
			static void run()
			{
				if (example_ != nullptr)
					return;
				zmq_threadstart(launch, nullptr);
			}
			/**
			* \brief ��Ϣ��
			*/
			static void launch(void*);

			/**
			*\brief �㲥����
			*/
			static bool monitor(const string& publiher, const string& state, const string& content)
			{
				if (example_ != nullptr)
					return example_->publish(publiher, state, content);
				return false;
			}
		};
	}
}
#endif