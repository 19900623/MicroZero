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
			* \brief �������վ��
			*/
			static void clear();
			/**
			* \brief ��ԭվ��
			*/
			static int restore();
			/**
			* \brief ��װһ��վ��
			*/
			static bool install( const char* station_name,int station_type, acl::string& config);

			/**
			* \brief վ��ж��
			*/
			static	bool uninstall_station(const string& stattion);

			/**
			* \brief ��ԭվ��
			*/
			static bool restore(acl::string& value);
			/**
			* \brief ����վ��
			*/
			static bool join(zero_station* station);
			/**
			* \brief ����վ��
			*/
			static bool left(zero_station* station);
			/**
			* \brief ����������վ��
			*/
			static zero_station* instance(const string& name);
		};
	}
}
#endif
