#pragma once
#include "stdafx.h"
#include "redis/entity_redis.h"
//�������״̬
typedef int TRADECOMMAND_STATUS;
//δ��ʼ��
const TRADECOMMAND_STATUS TRADECOMMAND_STATUS_NEED_INITIALIZE = -1;
//����
const TRADECOMMAND_STATUS TRADECOMMAND_STATUS_SUCCEED = 0;
//�������޷���
const TRADECOMMAND_STATUS TRADECOMMAND_STATUS_ES_SERVER_OPEN_FAILURE = 1;
//��������¼ʧ��
const TRADECOMMAND_STATUS TRADECOMMAND_STATUS_ES_LOGIN_FAILURE = 2;

namespace agebull
{
	namespace Rpc
	{

		class MqCommand
		{
			//�������״̬
			TRADECOMMAND_STATUS m_state;
			//�����״̬
			TRADECOMMAND_STATUS m_last_trade_state;
		public:
			/**
			* @brief ����
			*/
			MqCommand()
				: m_state(TRADECOMMAND_STATUS_NEED_INITIALIZE)
				, m_last_trade_state(0)
			{
			}

			/**
			* @brief ����
			*/
			~MqCommand()
			{
				free();
			}
			/**
			* @brief �ر�
			*/
			void close() const
			{
			}
			/**
			* @brief ����
			*/
			void free() const
			{
			}
			/**
			* @brief �Ƿ�����
			*/
			bool succeed() const
			{
				return m_state == TRADECOMMAND_STATUS_SUCCEED;
			}


			/**
			* @brief ��ʼ��
			* @return �Ƿ�ɹ�
			*/
			bool Initialize()
			{
				m_state = TRADECOMMAND_STATUS_SUCCEED;

				return true;
			}
			/**
			* @brief ��Ϣ��
			* @param {PNetCommand} cmd �������
			*/
			void message_pump(NetCommandArgPtr& cmd_arg)
			{

				switch (cmd_arg->cmd_id)
				{
				case NET_COMMAND_SYSTEM_NOTIFY:
					OnSystemEvent(cmd_arg);
					return;
				case NET_COMMAND_BUSINESS_NOTIFY:
					OnBusinessEvent(cmd_arg);
					return;
				case NET_COMMAND_CALL:
					OnCall(cmd_arg);
					return;
				case NET_COMMAND_RESULT:
					OnResult(cmd_arg);
					return;
				case NET_COMMAND_FLOW_START:
					OnResult(cmd_arg);
					return;
				case NET_COMMAND_FLOW_END:
					OnResult(cmd_arg);
					return;
				case NET_COMMAND_FLOW_STEP_SUCCEESS:
					OnResult(cmd_arg);
					return;
				case NET_COMMAND_FLOW_STEP_FAILED:
					OnResult(cmd_arg);
					return;
				case NET_COMMAND_FLOW_STTATE:
					OnResult(cmd_arg);
					return;
				}
			}

		private:
			/**
			* @brief ϵͳ�¼��ѽ���
			* @param {NetCommandArgPtr&} cmd_arg �������
			* @return ��
			*/
			void OnSystemEvent(NetCommandArgPtr& cmd_arg)
			{
				//save_cmd_arg(cmd_arg.m_command);
				notify_message(cmd_arg);
			}
			/**
			* @brief ҵ���¼��ѷ���
			* @param {NetCommandArgPtr&} cmd_arg �������
			* @return ��
			*/
			void OnBusinessEvent(NetCommandArgPtr& cmd_arg)
			{
				//save_cmd_arg(cmd_arg.m_command);
				notify_message(cmd_arg);
			}
			/**
			* @brief ��������
			* @param {NetCommandArgPtr&} cmd_arg �������
			* @return ��
			*/
			void OnCall(NetCommandArgPtr& cmd_arg)
			{
				save_cmd_arg(cmd_arg.m_command);
				notify_message(cmd_arg);
			}
			/**
			* @brief ��������
			* @param {NetCommandArgPtr&} cmd_arg �������
			* @return ��
			*/
			void OnResult(NetCommandArgPtr& cmd_arg)
			{
				delete_cmd_arg(cmd_arg.m_command);
				cmd_arg->cmd_state = NET_COMMAND_STATE_SUCCEED;
				command_answer(cmd_arg);
			}
			/**
			* @brief ��������
			* @param {NetCommandArgPtr&} cmd_arg �������
			* @return ��
			*/
			void OnFlow(NetCommandArgPtr& cmd_arg)
			{
				delete_cmd_arg(cmd_arg.m_command);
				command_answer(cmd_arg);
			}
		};
	}
}
