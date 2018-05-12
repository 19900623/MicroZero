#ifndef _AGEBULL_NET_COMMAND_TRADECOMMAND_CLIENT_H
#define _AGEBULL_NET_COMMAND_TRADECOMMAND_CLIENT_H
#pragma once
#include "stdafx.h"
#ifdef CLR
#pragma unmanaged
#endif

#include "entity.h"
using namespace Agebull::Futures::Globals;

namespace Agebull
{
	namespace Futures
	{
		namespace Globals
		{
			namespace Client
			{
				/**
				* @brief ��ѯ������״̬
				* @return ��
				*/
				COMMAND_STATE QryExchangeState();

				/**
				* @brief ��ѯ������Ʒ
				* @return ��
				*/
				COMMAND_STATE QryCommodity();

				/**
				* @brief ��ѯ��Լ
				* @param {ContractQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryContract(ContractQueryArg& arg);

				/**
				* @brief ��ѯ���ұ�����Ϣ
				* @return ��
				*/
				COMMAND_STATE QryCurrency();
#ifndef WEB
				/**
				* @brief �޸ĵ�ǰƷ��
				* @return ��
				*/
				COMMAND_STATE ChangeCommodity(const char* commodity);


				/**
				* @brief ��������
				* @param {OrderInsertArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE OrderInsert(CustomerOrder& arg);
				/**
				* @brief ��ѯ�ͻ��ʽ�
				* @return ��
				*/
				COMMAND_STATE QryMoney();

				/**
				* @brief ��ѯ�ͻ�ί��
				* @param {OrderQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryOrder(OrderQueryArg& arg);

				/**
				* @brief ��ѯ�ͻ��ɽ�
				* @param {TMatchQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryMatch(TMatchQueryArg& arg);

				/**
				* @brief ��ѯ�ֲ�
				* @param {HoldQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryHold(HoldQueryArg& arg);

				/**
				* @brief ��ѯK������
				* @return ��
				*/
				COMMAND_STATE QryCandleStick(StringArgument& arg);
				/**
				* @brief �ĵ�����
				* @param {OrderModifyArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE OrderModify(OrderModifyArg& arg);

				/**
				* @brief ��������
				* @param {int} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE OrderDelete(int& arg);

				/**
				* @brief ��ʷί�в�ѯ
				* @param {HisOrderQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryHistOrder(HisOrderQueryArg& arg);

				/**
				* @brief ��ʷ�ɽ���ѯ
				* @param {HisMatchQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryHistMatch(HisMatchQueryArg& arg);

				/**
				* @brief ��������ѯ
				* @return ��
				*/
				COMMAND_STATE QryCashOper();

				/**
				* @brief ��������������
				* @param {CashCheckArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE CashCheck(CashCheckArg& arg);

				/**
				* @brief �ʽ������ѯ
				* @return ��
				*/
				COMMAND_STATE QryCachAdjust();

				/**
				* @brief ��ʷ��������ѯ
				* @param {HisCashOperQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryHistCashOper(HisCashOperQueryArg& arg);

				/**
				* @brief ��ʷ�ʽ������ѯ
				* @param {HisAdjustQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryHistCachAdjust(HisAdjustQueryArg& arg);

				/**
				* @brief �¸۽��������̱���
				* @param {HKMarketOrderArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE HKMarketOrderOperator(HKMarketOrderArg& arg);

				/**
				* @brief �ͻ����������ѯ
				* @param {TClientCountRentQueryArg} arg ���ò���
				* @return ��
				*/
				COMMAND_STATE QryCountRent(TClientCountRentQueryArg& arg);
#endif
			}
		}
	}
}
#endif
