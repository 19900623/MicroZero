#pragma once
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{

		class ApiStation :public RouteStation<ApiStation, string, STATION_TYPE_API>
		{
			/**
			* @brief API��������
			*/
			vector<string> _hosts;

			/**
			* @brief ��ǰ�������±�
			*/
			int _nowWorkIndex;
		public:
			/**
			* @brief ����
			*/
			ApiStation(string name);
			/**
			* @brief ����
			*/
			virtual ~ApiStation() {}
			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				ApiStation* netobj = new ApiStation(name);
				boost::thread thrds_s1(boost::bind(start, shared_ptr<ApiStation>(netobj)));
			}
			/**
			* @brief ִ��
			*/
			static void start(shared_ptr<ApiStation> netobj)
			{
				station_run(netobj);
			}
		private:
			/**
			* @brief �������ϵ���Ӧ
			*/
			void onWorkerPollIn() override;
			/**
			* @brief ���ü��ϵ���Ӧ
			*/
			void onCallerPollIn() override;
			/**
			* @brief �������ϵ���Ӧ
			*/
			string create_item(const char* addr, const char * value)override
			{
				return value;
			}
			void ApiStation::left(char* name)override;
			/**
			* @brief ���͵�������
			*/
			bool snedToWorker(char* work, char* client_addr, char* request);

			/**
			* @brief ȡ��һ����������
			*/
			char* getNextWorker();
		};
	}
}