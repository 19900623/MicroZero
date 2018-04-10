#pragma once
#ifndef  _AGEBULL_station_warehouse_H_
#include <stdinc.h>
namespace agebull
{
	namespace zmq_net
	{
		class zero_station;

		/**
		* \brief ����վ��ʵ������վ��ֿ⣬�ǲ��Ǻ��Զ������֣�
		*/
		class station_warehouse
		{
			/**
			* \brief ʵ�����з�����
			*/
			static boost::mutex mutex_;
		public:
			/**
			* \brief ʵ������
			*/
			static map<string, zero_station*> examples_;
			/**
			* \brief ������з���
			*/
			static void clear();
			/**
			* \brief ��ԭ����
			*/
			static int restore();
			/**
			* \brief ��ʼ������
			*/
			static acl::string install(int station_type, const char* station_name);
			/**
			* \brief ��ԭ����
			*/
			static bool restore(acl::string& value);
			/**
			* \brief �������
			*/
			static bool join(zero_station* station);
			/**
			* \brief �������
			*/
			static bool left(zero_station* station);
			/**
			* \brief �������
			*/
			static zero_station* find(const string& name);
		};
	}
}
#endif
