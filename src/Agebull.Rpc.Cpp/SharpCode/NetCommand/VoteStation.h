#pragma once
#include <stdinc.h>
#include "NetStation.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		 * ͶƱ�߶���
		 */
		typedef struct
		{
			/**
			* ��������
			*/
			string flow_name;
			/**
			* ��������
			*/
			string net_name;
			/**
			 * �Ƿ����
			 */
			bool must;
			/**
			* �Ƿ�Ԥ����
			*/
			bool predefinition;
			/**
			 * �Ƿ����
			 */
			bool ready;
		}Voter, *PVoter;

		/**
		* @brief ��ʾһ������ͶƱվ������������й�������
		* ͶƱ�����ʽ��
		* 1 �����ʶ��������GUID/UUID��
		* 2 ����״̬
		* ��voters ����ͶƱ������, ����������JSON��ʽ������
		* ��start ��ʼͶƱ
		* ��continue �����ȴ�ͶƱ���������յ�ͶƱ�߷����ķ�Ӧ
		* ��end ���ͶƱ, ����bye
		* ��bad ͶƱʧ��, ����bye
		* 3 �������
		* ͶƱ���ظ�ʽ��
		* 1 �����ʶ
		* 2 ͶƱ�ߣ�*��ʾϵͳ���м��ˣ�
		* 3 ͶƱ���������ʹ��JSON��ʽ��
		*/
		class VoteStation :public RouteStation<VoteStation, Voter, STATION_TYPE_VOTE>
		{
		public:
			/**
			* @brief ����
			*/
			VoteStation(string name);
			/**
			* @brief ����
			*/
			virtual ~VoteStation() {}
		private:
			Voter create_item(const char* addr, const char * value) override
			{
				config cfg(value);
				Voter vote;
				vote.net_name = addr;
				vote.ready = true;
				vote.flow_name = cfg["flow_name"];
				vote.must = cfg["must"] == "true";
				vote.predefinition = cfg["predefinition"] == "true";
				return vote;
			}
			/**
			* @brief �������ϵ���Ӧ
			*/
			void onWorkerPollIn()override;
			/**
			* @brief ���ü��ϵ���Ӧ
			*/
			void onCallerPollIn() override;

			/**
			* @brief ����������ͶƱ״̬
			*/
			bool send_state(const char* client_addr, const  char* request_token, const char* voter, const char* state);

			/**
			* @brief ��ʼͶƱ
			*/
			bool start_vote(const char* client_addr, const  char* request_token, const  char* request_argument);
			/**
			* @brief ����������ͶƱ���б�
			*/
			bool get_voters(const char* client_addr, const  char* request_token);

		public:
			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				VoteStation* route = new VoteStation(name);
				boost::thread thrds_s1(boost::bind(VoteStation::start, shared_ptr<VoteStation>(route)));
			}
			/**
			* @brief ִ��
			*/
			static void start(shared_ptr<VoteStation> netobj)
			{
				station_run(netobj);
			}
		};
	}
}