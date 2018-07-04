#pragma once
#ifdef SERVER
#include "net_command.h"

#ifndef PROXY


/**
* @brief ��¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserLogin(const PNetCommand cmd);

/**
* @brief �ǳ�
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserLogout(const PNetCommand cmd);

/**
* @brief �޸�����
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserSetPassword(const PNetCommand cmd);
#endif

/**
* @brief ��ѯ������
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryExchange(const PNetCommand cmd);

/**
* @brief ��ѯ������Ʒ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryCommodity(const PNetCommand cmd);

/**
* @brief ��ѯ��Լ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryContract(const PNetCommand cmd);

/**
* @brief ��ѯ���ұ�����Ϣ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryCurrency(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ��ʽ��¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryMoney(const PNetCommand cmd);

/**
* @brief ��ѯ�ֲּ�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHold(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ�����ί�м�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayOrder(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ���ʷί�м�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryOrder(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ����ճɽ���¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayMatch(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ���ʷ�ɽ���¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryMatch(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ����ճ�����¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayCash(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ���ʷ������¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryCash(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ��ʽ��յ�����¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayCashAdjust(const PNetCommand cmd);

/**
* @brief ��ѯ�ͻ��ʽ���ʷ������¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryCashAdjust(const PNetCommand cmd);

/**
* @brief ��������
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderInsert(const PNetCommand cmd);

/**
* @brief �ĵ�����
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderModify(const PNetCommand cmd);

/**
* @brief ��������
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderCancel(const PNetCommand cmd);

#endif
