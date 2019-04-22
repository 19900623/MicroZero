#pragma once
#ifndef _PROXY_DISPATCHER_H_
#define _PROXY_DISPATCHER_H_
#include "../stdinc.h"
#include "zero_station.h"
#include "inner_socket.h"

#ifdef PROXYSTATION

namespace agebull
{
	namespace zero_net
	{
		/**
		* \brief ����ڵ�
		*/
		struct proxy_item
		{
			/**
			* \brief ����
			*/
			std::string name;
			/**
			* \brief ���ص�ַ
			*/
			std::string res_addr;
			/**
			* \brief �����ַ
			*/
			std::string req_addr;
			/**
			* \brief ������
			*/
			zmq_handler req_socket;
			/**
			* \brief ���ؾ��
			*/
			zmq_handler res_socket;

			proxy_item(): req_socket(nullptr), res_socket(nullptr)
			{
			}
		};
		/**
		* \brief ��ʾ�ƻ�������ȷ���
		*/
		class proxy_dispatcher :public zero_station
		{
			map<std::string, proxy_item> proxys_;
		public:
			/**
			* \brief ����
			*/
			static proxy_dispatcher* instance;

			/**
			* \brief ����
			*/
			proxy_dispatcher()
				: zero_station(zero_def::name::proxy_dispatcher, zero_def::station_type::proxy, ZMQ_ROUTER, ZMQ_PUB)
			{
			}

			/**
			* \brief ����
			*/
			proxy_dispatcher(shared_ptr<station_config>& config)
				: zero_station(config, zero_def::station_type::plan, ZMQ_ROUTER, ZMQ_PUB)
			{
			}

			/**
			* \brief ����
			*/
			~proxy_dispatcher() override
			{
				cout << "queue_station destory" << endl;
			}
			/**
			* \brief ����һ��֪ͨ�߳�
			*/
			static void run()
			{
				instance = new proxy_dispatcher();
				boost::thread(boost::bind(launch, shared_ptr<proxy_dispatcher>(instance)));
			}
		private:
			/**
			*\brief ����
			*/
			static void run(shared_ptr<station_config>& config)
			{
				instance = new proxy_dispatcher(config);
				boost::thread(boost::bind(launch, shared_ptr<proxy_dispatcher>(instance)));
			}
			/**
			* \brief ��Ϣ��
			*/
			static void launch(shared_ptr<proxy_dispatcher>& station);


			/**
			* \brief �ƻ���ѯ
			*/
			static void run_proxy_poll(proxy_dispatcher* station)
			{
				station->proxy_poll();
			}

			/**
			* \brief �ƻ���ѯ
			*/
			void proxy_poll();

			/**
			* \brief ����
			*/
			bool on_start(zmq_handler socket, shared_char name, vector<shared_char>& list);

			/**
			* \brief ������ʼ : ������������
			*/
			void job_start(zmq_handler socket, vector<shared_char>& list, bool inner, bool old) final;

			/**
			* \brief ��������(���͵�������)
			*/
			void job_end(vector<shared_char>& list) final;

			/**
			* \brief ����
			*/
			void on_result(zmq_handler socket, const char* caller, uchar state);

		};
	}
}

#endif // PROXYSTATION
#endif //!_PROXY_DISPATCHER_H_