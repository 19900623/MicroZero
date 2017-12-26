#include "MqCommand.h"
#include "Business/DataNotifyBusiness.h"

namespace Agebull
{
	namespace Futures
	{
		namespace TradeManagement
		{
			/**
			* @brief ����
			*/
			MqCommand::MqCommand()
				: m_state(TRADECOMMAND_STATUS_NEED_INITIALIZE)
				, m_last_trade_state(0)
			{
			}

			/**
			* @brief ����
			*/
			MqCommand::~MqCommand()
			{
				free();
			}


			/**
			* @brief ��ʼ��
			* @return �Ƿ�ɹ�
			*/
			bool MqCommand::Initialize()
			{
				m_state = TRADECOMMAND_STATUS_SUCCEED;
				return true;
			}

			/**
			* @brief ��Ϣ��
			* @param {PNetCommand} cmd �������
			*/
			void MqCommand::message_pump(NetCommandArgPtr& cmd_arg)
			{
				switch (cmd_arg->cmd_id)
				{
				case NET_COMMAND_USER_SET_PASSWORD://�޸Ŀͻ�����
					UserSetPassword(cmd_arg);
					return;
				case NET_COMMAND_USER_LOGIN://��¼
					UserLogin(cmd_arg);
					return;
				case NET_COMMAND_USER_CHANGE_COMMODITY://�ı䵱ǰƷ��
					ChangedCommodity(cmd_arg);
					return;
				case NET_COMMAND_USER_LOGOUT://�ǳ�
					UserLogout(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYMONEY://��ѯ�ͻ��ʽ�
					DoQryMoney(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYORDER://��ѯ�ͻ�ί��
					DoQryOrder(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYMATCH://��ѯ�ͻ��ɽ�
					DoQryMatch(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYHOLD://��ѯ�ֲ�
					DoQryHold(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYEXCHANGESTATE://��ѯ������״̬
					DoQryExchangeState(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYCOMMODITY://��ѯ������Ʒ
					DoQryCommodity(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYCONTRACT://��ѯ��Լ
					DoQryContract(cmd_arg);
					return;
				case NET_COMMAND_ES_ORDERINSERT://��������
					DoOrderInsert(cmd_arg);
					return;
				case NET_COMMAND_ES_ORDERMODIFY://�ĵ�����
					DoOrderModify(cmd_arg);
					return;
				case NET_COMMAND_ES_ORDERDELETE://��������
					DoOrderDelete(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYHISTORDER://��ʷί�в�ѯ
					DoQryHistOrder(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYHISTMATCH://��ʷ�ɽ���ѯ
					DoQryHistMatch(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYCASHOPER://��������ѯ
					DoQryCashOper(cmd_arg);
					return;
				case NET_COMMAND_ES_CASHCHECK://��������������
					DoCashCheck(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYCACHADJUST://�ʽ������ѯ
					DoQryCachAdjust(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYHISTCASHOPER://��ʷ��������ѯ
					DoQryHistCashOper(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYHISTCACHADJUST://��ʷ�ʽ������ѯ
					DoQryHistCachAdjust(cmd_arg);
					return;
				case NET_COMMAND_ES_HKMARKETORDEROPERATOR://�¸۽��������̱���
					DoHKMarketOrderOperator(cmd_arg);
					return;
				case NET_COMMAND_ES_QRYCOUNTRENT://�ͻ����������ѯ
					DoQryCountRent(cmd_arg);
					return;
				case NET_COMMAND_ES_QRQUOTE://��ѯ��ʷ����
					DoQryCandleStick(cmd_arg);
					return;
				default:
					DataNotifyBusiness::Notify(cmd_arg.m_command);
					break;
				}
			}

		}
	}
}
