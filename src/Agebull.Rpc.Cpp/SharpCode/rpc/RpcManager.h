#pragma once
#ifndef RPC_MANAGER_H
namespace agebull
{
	namespace Rpc
	{
		/*
		* @briref ��������
		*/
		char* mathine_handle(const char* name,const char* key);
		/*
		* @briref ����ע��
		*/
		void mathine_regist();
		/*
		* @briref ��������ͬ��
		*/
		void mathine_sync_config();
		/*
		* @briref ��������
		*/
		void mathine_heart();
		/*
		* @briref ��������
		*/
		void mathine_call();
		/*
		* @briref �����ػ�
		*/
		void mathine_shutdown();
		/*
		* @briref ��������
		*/
		void mathine_up_down();
		/*
		* @briref ��������
		*/
		void mathine_discard();
	}
}
#endif