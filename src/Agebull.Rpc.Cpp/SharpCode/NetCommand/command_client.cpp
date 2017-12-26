#include "stdafx.h"
#include "command_client.h"
#include <TradeCommon/TradeCommand.h>
#include <log/log.h>
#ifdef CLR
#pragma unmanaged
#endif

namespace Agebull
{
	namespace Futures
	{
		namespace Globals
		{
			namespace Client
			{
				/**
				* @brief ��ѯ���ұ�����Ϣ
				* @return ��
				*/
				COMMAND_STATE QryCurrency()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCURRENCY);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ������״̬
				* @return ��
				*/
				COMMAND_STATE QryExchangeState()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYEXCHANGESTATE);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ������Ʒ
				* @return ��
				*/
				COMMAND_STATE QryCommodity()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCOMMODITY);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ��Լ
				* @return ��
				*/
				COMMAND_STATE QryContract(ContractQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCONTRACT);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
#ifndef WEB
				/**
				* @brief �޸ĵ�ǰƷ��
				* @return ��
				*/
				COMMAND_STATE ChangeCommodity(const char* commodity)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;

					//�������
					if (commodity == nullptr || strlen(commodity) == 0)
					{
						log_msg1("�޸ĵ�ǰƷ��:%s,��ΪƷ��Ϊ�ն�ʧ��", commodity);
						return NET_COMMAND_STATE_ARGUMENT_INVALID;
					}
					log_msg1("�޸ĵ�ǰƷ��:%s", commodity);
					StringArgument args;
					memset(&args, 0, sizeof(StringArgument));
					strcpy_s(args.Argument, commodity);


					PNetCommand net_cmd = SerializeToCommand(&args);
					set_command_head(net_cmd, NET_COMMAND_USER_CHANGE_COMMODITY);
					try
					{
						//TO DO:������
						request_net_cmmmand(net_cmd);
					}
					catch (...)
					{
						log_msg1("�޸ĵ�ǰƷ��:%s,�����쳣", commodity);
						delete[] net_cmd;
					}
					return state;
				}
				/**
				* @brief ��ѯ�ͻ��ʽ�
				* @return ��
				*/
				COMMAND_STATE QryMoney()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYMONEY);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ�ͻ�ί��
				* @return ��
				*/
				COMMAND_STATE QryOrder(OrderQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYORDER);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ�ͻ��ɽ�
				* @return ��
				*/
				COMMAND_STATE QryMatch(TMatchQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYMATCH);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯ�ֲ�
				* @return ��
				*/
				COMMAND_STATE QryHold(HoldQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYHOLD);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��������
				* @return ��
				*/
				COMMAND_STATE OrderDelete(int& arg)
				{
					log_msg1("��������:%d", arg);
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						StringArgument str_arg;
						sprintf_s(str_arg.Argument, "%d", arg);
						auto cmd_call = SerializeToCommand(&str_arg);
						set_command_head(cmd_call, NET_COMMAND_ES_ORDERDELETE);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						log_msg1("��������:%d,�����쳣", arg);
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��������
				* @return ��
				*/
				COMMAND_STATE OrderInsert(CustomerOrder& arg)
				{
					log_msg5("��������:%s%s%s%d��%s", arg.CommodityNo, arg.ContractNo, (to_log_text(arg.Direct).c_str()), arg.OrderVol, (arg.HoldId > 0 ? "ƽ��" : "����"));
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_ORDERINSERT);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						log_msg5("��������:%s%s%s%d��%s,�����쳣", arg.CommodityNo, arg.ContractNo, (to_log_text(arg.Direct).c_str()), arg.OrderVol, (arg.HoldId > 0 ? "ƽ��" : "����"));
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief �ĵ�����
				* @return ��
				*/
				COMMAND_STATE OrderModify(OrderModifyArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_ORDERMODIFY);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ʷί�в�ѯ
				* @return ��
				*/
				COMMAND_STATE QryHistOrder(HisOrderQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYHISTORDER);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ʷ�ɽ���ѯ
				* @return ��
				*/
				COMMAND_STATE QryHistMatch(HisMatchQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYHISTMATCH);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��������ѯ
				* @return ��
				*/
				COMMAND_STATE QryCashOper()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCASHOPER);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��������������
				* @return ��
				*/
				COMMAND_STATE CashCheck(CashCheckArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_CASHCHECK);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief �ʽ������ѯ
				* @return ��
				*/
				COMMAND_STATE QryCachAdjust()
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						PNetCommand cmd_call = new NetCommand();
						cmd_call->data_len = 0;
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCACHADJUST);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ʷ��������ѯ
				* @return ��
				*/
				COMMAND_STATE QryHistCashOper(HisCashOperQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYHISTCASHOPER);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ʷ�ʽ������ѯ
				* @return ��
				*/
				COMMAND_STATE QryHistCachAdjust(HisAdjustQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYHISTCACHADJUST);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief �¸۽��������̱���
				* @return ��
				*/
				COMMAND_STATE HKMarketOrderOperator(HKMarketOrderArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_HKMARKETORDEROPERATOR);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief �ͻ����������ѯ
				* @return ��
				*/
				COMMAND_STATE QryCountRent(TClientCountRentQueryArg& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRYCOUNTRENT);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
				/**
				* @brief ��ѯK������
				* @return ��
				*/
				COMMAND_STATE QryCandleStick(StringArgument& arg)
				{
					COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
					try
					{
						auto cmd_call = SerializeToCommand(&arg);
						set_command_head(cmd_call, NET_COMMAND_ES_QRQUOTE);
						request_net_cmmmand(cmd_call);
					}
					catch (...)
					{
						state = NET_COMMAND_STATE_SERVER_UNKNOW;
					}
					return state;
				}
#endif
			}
		}
	}
}