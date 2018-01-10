#pragma once
#ifndef _ZMQ_NET_BALANCESTATION_H_
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{

		/**
		* \brief ��ʾһ������ZMQ�ĸ��ؾ���վ��
		* \tparam TNetStation
		* \tparam TWorker
		* \tparam NetType
		*/
		template <typename TNetStation, class TWorker, int NetType>
		class BalanceStation : public NetStation
		{
		protected:
			/**
			* @brief �����߼���
			*/
			map<string, TWorker> _workers;
		public:
			/**
			* @brief ����
			*/
			BalanceStation(string name)
				: NetStation(name, NetType, ZMQ_ROUTER, ZMQ_ROUTER, ZMQ_ROUTER)
			{
			}

		protected:

			/**
			* \brief ���ɹ�������
			*/
			virtual TWorker create_item(const char* addr, const char* value) = 0;

			/**
			* ��������Ӧ
			*/
			void heartbeat() override;
			/**
			* @brief ���������˳�
			*/
			virtual void worker_left(const char* addr);

			/**
			* @brief �����������
			*/
			virtual void worker_join(const char* addr, const  char* value, bool ready = false);
		};


		/**
		* \brief
		*/
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::heartbeat()
		{
			vector<sharp_char> list;
			//0 ·�ɵ��ĵ�ַ 1 ��֡ 2 ���� 3 ��������ַ
			_zmq_state = recv(_heart_socket, list);
			switch (list[2][0])
			{
			case '@':
				worker_join(*list[3], *list[3]);
				break;
			case '-':
				worker_left(*list[3]);
				break;
			default: break;
			}
			_zmq_state = send_addr(_heart_socket, *list[0]);
			_zmq_state = send_late(_heart_socket, "OK");
		}

		/**
		* \brief
		* \param addr
		*/
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::worker_left(const char* addr)
		{
			auto vote = _workers.find(addr);
			if (vote != _workers.end())
			{
				_workers.erase(addr);
				monitor(_station_name, "worker_left", addr);
			}
		}

		/**
		* \brief
		* \param addr
		* \param value
		* \param ready
		*/
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::worker_join(const char* addr, const  char* value, bool ready)
		{
			TWorker item = create_item(addr, value);
			auto old = _workers.find(addr);
			if (old == _workers.end())
			{
				_workers.insert(make_pair(addr, item));
				monitor(_station_name, "worker_join", addr);
			}
			else
			{
				old->second = item;
				monitor(_station_name, "worker_heart", addr);
			}
		}

	}
}
#endif
