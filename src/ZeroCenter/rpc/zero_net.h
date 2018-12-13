#pragma once
#ifndef ZERO_NET_H_
#define ZERO_NET_H_
#include "zero_default.h"
namespace agebull
{
	namespace zero_net
	{



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
		*\brief ϵͳ�¼�֪ͨ
		*/
		bool system_event(zero_net_event event_type, const char* sub = nullptr, const char* content = nullptr);

		/**
		*\brief �¼�֪ͨ
		*/
		bool zero_event(zero_net_event event_type, const char* title, const char* sub, const char* content);

		
	}
}
#endif