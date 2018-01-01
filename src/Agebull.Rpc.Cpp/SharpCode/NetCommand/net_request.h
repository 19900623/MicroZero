#ifndef _AGEBULL_NET_COMMAND_REQUEST_H
#define _AGEBULL_NET_COMMAND_REQUEST_H
#pragma once
#include "stdinc.h"
#include "redis/redis.h"

#ifdef WIN32
#include <objbase.h>  
#else
#include <uuid/uuid.h>  
typedef uuid_t GUID;
#endif

#include "command_def.h"
#include <tson/tson_serializer.h>
#include <Tson/tson_deserializer.h>
#include "InnerTrade/MqCommand.h"

using namespace std;


namespace agebull
{
	namespace Rpc
	{
		class CommandPump
		{
			//�������״̬
			TRADECOMMAND_STATUS m_state;
			//�����״̬
			TRADECOMMAND_STATUS m_last_trade_state;
		public:
			/**
			* @brief ����
			*/
			CommandPump()
				: m_state(TRADECOMMAND_STATUS_NEED_INITIALIZE)
				, m_last_trade_state(0)
			{
			}

			/**
			* @brief ����
			*/
			virtual ~CommandPump()
			{
			}
			/**
			* @brief �ر�
			*/
			void virtual close() const
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
			bool virtual Initialize()
			{
				m_state = TRADECOMMAND_STATUS_SUCCEED;

				return true;
			}

			/*
			* �󶨵�ַ
			*/
			void set_address(const char* addr)
			{
				strcpy_s(m_address, addr);
			}
			/**
			* @brief ��Ϣ��
			* @param {PNetCommand} cmd �������
			*/
			PNetCommand virtual message_pump(NetCommandArgPtr& cmd_arg);

			/**
			* @brief �����
			*/
			void request_pump();
		private:
			//������Ϣ����
			queue<NetCommandArgPtr> m_queue;
			//������Ϣ������
			boost::mutex m_lock;
			/*
			 * �󶨵�ַ
			 */
			char m_address[128];
		};
	}
}

#endif