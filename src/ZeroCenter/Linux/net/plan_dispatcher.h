#pragma once
#ifndef _PLAN_DISPATCHER_H_
#define _PLAN_DISPATCHER_H_
#include "../stdinc.h"
#include "zero_plan.h"
#include "zero_station.h"
#include "inner_socket.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		* \brief ��ʾ�ƻ�������ȷ���
		*/
		class plan_dispatcher :public zero_station
		{
			map<string, shared_ptr<inner_socket>> sockets_;
		public:
			/**
			* \brief ����
			*/
			static plan_dispatcher* instance;

			/**
			* \brief ����
			*/
			plan_dispatcher()
				:zero_station("PlanDispatcher", STATION_TYPE_PLAN, ZMQ_ROUTER)
			{

			}
			/**
			* \brief ����
			*/
			plan_dispatcher(shared_ptr<zero_config>& config)
				:zero_station(config, STATION_TYPE_PLAN, ZMQ_ROUTER)
			{

			}
			/**
			* \brief ����
			*/
			~plan_dispatcher() final = default;

			/**
			* \brief ����һ���㲥�߳�
			*/
			static void run()
			{
				instance = new plan_dispatcher();
				boost::thread(boost::bind(launch, shared_ptr<plan_dispatcher>(instance)));
			}
		private:
			/**
			*\brief ����
			*/
			static void run(shared_ptr<zero_config>& config)
			{
				instance = new plan_dispatcher(config);
				boost::thread(boost::bind(launch, shared_ptr<plan_dispatcher>(instance)));
			}
			/**
			* \brief ��Ϣ��
			*/
			static void launch(shared_ptr<plan_dispatcher>& station);


			/**
			* \brief �ƻ���ѯ
			*/
			static void run_plan_poll(plan_dispatcher* station)
			{
				station->plan_poll();
			}
			/**
			* \brief �ƻ���ѯ
			*/
			void plan_poll();

			/**
			* \brief ��ʼִ��һ������
			*/
			shared_char command(const char* caller, vector<shared_char> lines) final
			{
				return ZERO_STATUS_NET_ERROR;
			}

			/**
			*\brief ������Ϣ
			*/
			bool publish(plan_message& message, vector<shared_char>& result);
			/**
			* \brief ������ʼ�����͵������ߣ�
			*/
			void job_start(ZMQ_HANDLE socket, vector<shared_char>& list, bool inner) final;
			/**
			* \brief ��������(���͵�������)
			*/
			void job_end(vector<shared_char>& list) final
			{
			}
			/**
			* \brief ִ�мƻ�
			*/
			int exec_plan(plan_message& msg);
			/**
			* \brief �ƻ�ִ�з���
			*/
			bool on_plan_result(vector<shared_char>& list);
			/**
			* \brief �ƻ�����
			*/
			bool on_plan_start(ZMQ_HANDLE socket, vector<shared_char>& list);
		};
	}
}
#endif //!_PLAN_DISPATCHER_H_