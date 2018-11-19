#pragma once
#ifndef ZERO_NET_H_
#define  ZERO_NET_H_
#include "zero_default.h"
namespace agebull
{
	namespace zero_net
	{

		/**
		* \brief  վ��״̬
		*/
		enum class station_state
		{
			/**
			* \brief �ޣ��չ���
			*/
			none,
			/**
			* \brief ��������
			*/
			re_start,
			/**
			* \brief ��������
			*/
			start,
			/**
			* \brief ��������
			*/
			run,
			/**
			* \brief ����ͣ
			*/
			pause,
			/**
			* \brief ����״̬
			*/
			failed,
			/**
			* \brief ��Ҫ�ر�
			*/
			closing,
			/**
			* \brief �ѹر�
			*/
			closed,
			/**
			* \brief �����٣������ѵ���
			*/
			destroy,
			/**
			* \brief �ѹ�ͣ
			*/
			stop,
			/**
			* \brief δ֪
			*/
			unknow
		};


		/**
		*\brief ZMQ�����Ķ���
		*/
		zmq_handler get_zmq_context();

		/**
		*\brief ����״̬
		*/
		int get_net_state();

		/**
		*\brief �̼߳�������
		*/
		void reset_command_thread(int count);

		/**
		*\brief �Ǽ��߳�ʧ��
		*/
		void set_command_thread_bad(const char* name);

		/**
		*\brief �Ǽ��߳̿�ʼ
		*/
		void set_command_thread_run(const char* name);

		/**
		*\brief �Ǽ��̹߳ر�
		*/
		void set_command_thread_end(const char* name);

		/**
		*\brief �ȴ��ر�(����station_dispatcher����ʱʹ��һ��)
		*/
		void wait_close();

		/**
		*\brief  �����¼�
		*/
		enum class zero_net_event
		{
			/**
			*\brief
			*/
			event_none = 0x0,
			/**
			*\brief
			*/
			event_system_start = 0x1,
			/**
			*\brief
			*/
			event_system_closing,
			/**
			*\brief
			*/
			event_system_stop,
			/**
			*\brief
			*/
			event_worker_sound_off,
			/**
			*\brief
			*/
			event_station_join,
			/**
			*\brief
			*/
			event_station_left,
			/**
			*\brief
			*/
			event_station_pause,
			/**
			*\brief
			*/
			event_station_resume,
			/**
			*\brief
			*/
			event_station_closing,
			/**
			*\brief
			*/
			event_station_install,
			/**
			*\brief
			*/
			event_station_stop,
			/**
			*\brief
			*/
			event_station_remove,
			/**
			*\brief
			*/
			event_station_state,

			/**
			*\brief
			*/
			event_station_update,

			/**
			*\brief վ���ĵ�
			*/
			event_station_doc,

			/**
			*\brief �ͻ��˼���
			*/
			event_client_join,

			/**
			*\brief �ͻ����˳�
			*/
			event_client_left,

			/**
			*\brief �ƻ�����
			*/
			event_plan_add = 0x1,

			/**
			*\brief �ƻ�����
			*/
			event_plan_update,

			/**
			*\brief �ƻ��������
			*/
			event_plan_queue,

			/**
			*\brief �ƻ�����ִ��
			*/
			event_plan_exec,

			/**
			*\brief �ƻ�ִ�����
			*/
			event_plan_result,

			/**
			*\brief �ƻ���ͣ
			*/
			event_plan_pause,

			/**
			*\brief �ƻ��ѽ���
			*/
			event_plan_end,

			/**
			*\brief �ƻ���ɾ��
			*/
			event_plan_remove,

			/**
			*\brief �Զ����¼�
			*/
			event_custom = 0xFF
		};

		/**
		*\brief ϵͳ�¼�֪ͨ
		*/
		bool system_event(zero_net_event event_type, const char* sub = nullptr, const char* content = nullptr);

		/**
		*\brief �¼�֪ͨ
		*/
		bool zero_event(zero_net_event event_type, const char* title, const char* sub, const char* content);

		constexpr auto DEF_SYSTEM_MANAGE = "SystemManage";
		constexpr auto DEF_PLAN_DISPATCHER = "PlanDispatcher";
		constexpr auto DEF_PROXY_DISPATCHER = "ProxyDispatcher";
		constexpr auto DEF_HEAD_TCP = "+";
		constexpr auto DEF_HEAD_INPROC = '-';
		constexpr auto DEF_HEAD_REQ = '<';
		constexpr auto DEF_HEAD_RES = '>';
		
	}
}
#endif