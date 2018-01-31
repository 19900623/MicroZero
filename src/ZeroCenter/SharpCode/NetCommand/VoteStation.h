#ifndef ZMQ_API_VOTE_STATION_H
#pragma once
#include <stdinc.h>
 #include <utility>
#include "ZeroStation.h"
#include "BalanceStation.h"
#include "StationWarehouse.h"

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
		} Voter, *PVoter;

		/**
		* \brief ��ʾһ������ͶƱվ������������й�������
		* ͶƱ�����ʽ��
		* 1 �����ʶ��������GUID/UUID��
		* 2 ����״̬
		* ��* ����ͶƱ������, ����������JSON��ʽ������
		* ��@ ��ʼͶƱ
		* ��% �����ȴ�ͶƱ���������յ�ͶƱ�߷����ķ�Ӧ
		* ��v ���ͶƱ, ����bye
		* ��x ͶƱʧ��, ����bye
		* 3 �������
		* ͶƱ���ظ�ʽ��
		* 1 �����ʶ
		* 2 ͶƱ�ߣ�*��ʾϵͳ���м��ˣ�
		* 3 ͶƱ���������ʹ��JSON��ʽ��
		*/
		class VoteStation :public BalanceStation<VoteStation, Voter, STATION_TYPE_VOTE>
		{
		public:
			/**
			* \brief ����
			*/
			VoteStation(string name)
				: BalanceStation<VoteStation, Voter, STATION_TYPE_VOTE>(std::move(name))
			{
			}

			/**
			* \brief ����
			*/
			virtual ~VoteStation()
			{
			}

		private:
			Voter create_item(const char* addr, const char* value) override
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
			* \brief �������ϵ���Ӧ
			*/
			void response() override;
			/**
			* \brief ���ü��ϵ���Ӧ
			*/
			void request(ZMQ_HANDLE socket) override;

			/**
			* \brief ִ��һ������
			*/
			sharp_char command(const char* caller, vector<sharp_char> lines) override;
			/**
			* \brief ����������ͶƱ״̬
			*/
			bool send_state(const char* client_addr, const char* request_token, const char* voter, const char* state);

			/**
			* \brief ��ʼͶƱ
			*/
			bool start_vote(const char* client_addr, const char* request_token, const char* request_argument);
			/**
			* \brief ����������ͶƱ���б�
			*/
			bool get_voters(const char* client_addr, const char* request_token);

		public:
			/**
			*��Ϣ��
			*/
			static void run(string name)
			{
				VoteStation* route = new VoteStation(std::move(name));
				boost::thread thrds_s1(boost::bind(start, shared_ptr<VoteStation>(route)));
			}

			/**
			* \brief ִ��
			*/
			static void start(const shared_ptr<VoteStation>& arg);
		private:

			/**
			* \brief ִ��
			*/
			static void save(shared_ptr<VoteStation> arg);
		};
	}
}
#endif
