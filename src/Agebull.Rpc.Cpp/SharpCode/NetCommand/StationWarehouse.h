#pragma once
#ifndef  _AGEBULL_STATIONWAREHOUSE_H_
#include <stdinc.h>
namespace agebull
{
	namespace zmq_net
	{
		class ZeroStation;

		/**
		* @brief ����վ��ʵ������վ��ֿ⣬�ǲ��Ǻ��Զ������֣�
		*/
		class StationWarehouse
		{
			/**
			* @brief ʵ�����з�����
			*/
			static boost::mutex _mutex;
		public:
			/**
			* @brief ʵ������
			*/
			static map<string, ZeroStation*> examples;
			/**
			* @brief ������з���
			*/
			static void clear();
			/**
			* @brief ��ԭ����
			*/
			static int restore();
			/**
			* @brief ��ʼ������
			*/
			static acl::string install(int station_type, const char* station_name);
			/**
			* @brief ��ԭ����
			*/
			static bool restore(acl::string& value);
			/**
			* @brief �������
			*/
			static bool join(ZeroStation* station);
			/**
			* @brief �������
			*/
			static bool left(ZeroStation* station);
			/**
			* @brief �������
			*/
			static ZeroStation* find(string name);
		};
	}
}
#endif
