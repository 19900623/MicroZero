#ifndef ZMQ_API_VOTE_STATION_H
#pragma once
#include <stdinc.h>
#include <utility>
#include "ZeroStation.h"
#include "BalanceStation.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		 * ͶƱ�߶���
		 */
		struct voter
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
			bool must{ false };
			/**
			* �Ƿ�Ԥ����
			*/
			bool predefinition{ false };
			/**
			 * �Ƿ����
			 */
			bool ready{ false };
		};

		/**
		* \brief ��ʾһ������ͶƱվ������������й�������
		* ͶƱ�����ʽ��
		* 1 �����ʶ��������GUID/UUID��,����ȫ��Ψһ
		* 2 ����״̬
		* ��* ����ͶƱ������, ���أ�ͶƱ��Ϊ&,����������JSON��ʽ������
		* ��$ ȡͶƱ״̬,���أ�ͶƱ��Ϊ$, ��ÿ��ͶƱ�����ơ�״̬����һ֡������״̬�������֡Ϊ(* + �ܵ�״̬)��
		* ��@ ��ʼͶƱ,��ʱͶƱ״̬Ϊ-start,��������Ϊ��ʱ��ͶƱ״̬����ʽͬ$)��
		* ��% �����ȴ�ͶƱ����,���յ�ͶƱ�߷����ķ�Ӧ�����أ�ͶƱ��Ϊ*,����Ϊ-waiting
		* ��> ��δ���ͳɹ����޷��ص�ͶƱ����������,���أ�ͶƱ��Ϊ$, ��ÿ��ͶƱ�����ơ�״̬����һ֡������״̬�������֡Ϊ(* + �ܵ�״̬)��
		* ��v ���ͶƱ,���أ�ͶƱ��Ϊ*,����Ϊ-end
		* ��x ͶƱ�ر�, ���أ�ͶƱ��Ϊ*,����Ϊ-close
		* 3 �������
		* 
		* ͶƱ���ظ�ʽ��
		* 1 �����ʶ
		* 2 ͶƱ�ߣ�*��ʾϵͳ���м��ˣ�,$��ʾ����״̬��ϵͳ+����ͶƱ�ߣ���&��ʾ����Ϊ������ע��ͶƱ�ߣ���������Ϊ����ͶƱ��
		* 3 ���ݣ�һ����֡��*�����ͶƱ��Ϊһ֡�� $Ϊ��֡
		* 
		* ���ݴ洢��ʹ��Redis��Hash����ʽ���£�
		* 1 key: vote:[�����ʶ]
		* 2 field:
		*	* �ܵ�״̬��ֵ�У�-start,-waiting,-end,-close,+error
		*	# ͶƱ����
		*	@ ������
		*	[voter] ͶƱ��״̬��-send,+error����巵��ֵ
		*/
		class vote_station :public balance_station<vote_station, voter, STATION_TYPE_VOTE>
		{
		public:
			/**
			* \brief ����
			*/
			explicit vote_station(string name)
				: balance_station<vote_station, voter, STATION_TYPE_VOTE>(std::move(name))
			{
			}

			/**
			* \brief ����
			*/
			virtual ~vote_station()
				= default;

		private:
			/**
			 * \brief ����ڵ�
			 * \param addr 
			 * \param value 
			 * \return 
			 */
			voter create_item(const char* addr, const char* value) override
			{
				config cfg(value);
				voter v;
				v.net_name = addr;
				v.ready = true;
				v.flow_name = cfg["flow_name"];
				v.must = cfg["must"] == "true";
				v.predefinition = cfg["predefinition"] == "true";
				return v;
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
			* \brief ����������ͶƱ״̬
			*/
			bool send_state(const char* client_addr, const char* request_token, const char* state, std::map<acl::string, acl::string>& values);

			/**
			* \brief ��ʼͶƱ
			*/
			bool start_vote(const char* client_addr, const char* request_token, const char* request_argument);
			/**
			* \brief ��δͶƱ����������ͶƱ
			*/
			bool re_push_vote(const char* client_addr, const char* request_token);
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
				vote_station* route = new vote_station(std::move(name));
				boost::thread thrds_s1(boost::bind(launch, shared_ptr<vote_station>(route)));
			}

			/**
			* \brief ִ��
			*/
			static void launch(const shared_ptr<vote_station>& arg);
		};
	}
}
#endif
