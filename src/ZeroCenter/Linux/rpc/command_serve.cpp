#include "stdafx.h"
#ifdef SERVER
#include "SysCmdArg/LoginArg.h"
using namespace Agebull::Tson;
using namespace Agebull::Futures::Globals::DataModel;
using namespace Agebull::Futures::Globals;

//S�˵��ڴ���ն���
queue<void*> money_queue;

//ZMQ��Ҫ���ڴ�����
void free_cmd_money(void* data_ptr, void* base_ptr)
{
	//while ((money_queue.size() % 50) == 0)
	//{
	//	while (money_queue.size() > 25)
	//	{
	//		delete[] reinterpret_cast<char*>(money_queue.front());
	//		money_queue.pop();
	//	}
	//}
	//money_queue.push(base_ptr);
}
#ifndef PROXY
/**
* @brief ��¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnUserLogin(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = state;
	server_message_send(result);
}

/**
* @brief ��¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserLogin(const PNetCommand cmd)
{
	const boost::format fmt_def = boost::format("cus:%s:pwd");
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		deserialize_cmd_arg(cmd,LoginArg, cmd_arg);//��������Ϊcmd_arg
		boost::format fmt(fmt_def);
		fmt % cmd_arg.UserName;
		auto pwd = read_str_from_redis(fmt.str().c_str());
		if (pwd.empty() || !pwd.equal(cmd_arg.PassWord))
		{
			state = NET_COMMAND_STATE_ARGUMENT_INVALID;//�û������벻��
		}
		else
		{
			acl::string id = read_str_from_redis("r:lc:cus:name:%s", cmd_arg.UserName);
			write_to_redis(id.c_str(),id.length(),"r:lc:cus:token:%s", cmd->user_token);
		}
	}
	catch (...)
	{
		state = NET_COMMAND_STATE_SERVER_UNKNOW;
	}

	OnUserLogin(cmd, state);
}

/**
* @brief �ǳ���ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnUserLogout(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief �ǳ�
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserLogout(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnUserLogout(cmd, state);
}

/**
* @brief �޸�������ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnUserSetPassword(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief �޸�����
* @param {PNetCommand} cmd �������
* @return ��
*/
void UserSetPassword(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnUserSetPassword(cmd, state);
}

#endif

/**
* @brief ��ѯ��������ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnSysQueryExchange(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ������
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryExchange(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnSysQueryExchange(cmd, state);
}

/**
* @brief ��ѯ������Ʒ��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnSysQueryCommodity(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ������Ʒ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryCommodity(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnSysQueryCommodity(cmd, state);
}

/**
* @brief ��ѯ��Լ��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnSysQueryContract(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ��Լ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryContract(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnSysQueryContract(cmd, state);
}

/**
* @brief ��ѯ���ұ�����Ϣ��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnSysQueryCurrency(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ���ұ�����Ϣ
* @param {PNetCommand} cmd �������
* @return ��
*/
void SysQueryCurrency(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnSysQueryCurrency(cmd, state);
}

/**
* @brief ��ѯ�ͻ��ʽ��¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryMoney(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ��ʽ��¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryMoney(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryMoney(cmd, state);
}

/**
* @brief ��ѯ�ֲּ�¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryHold(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ֲּ�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHold(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryHold(cmd, state);
}

/**
* @brief ��ѯ�ͻ�����ί�м�¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryTodayOrder(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ�����ί�м�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayOrder(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryTodayOrder(cmd, state);
}

/**
* @brief ��ѯ�ͻ���ʷί�м�¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryHistoryOrder(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ���ʷί�м�¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryOrder(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryHistoryOrder(cmd, state);
}

/**
* @brief ��ѯ�ͻ����ճɽ���¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryTodayMatch(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ����ճɽ���¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayMatch(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryTodayMatch(cmd, state);
}

/**
* @brief ��ѯ�ͻ���ʷ�ɽ���¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryHistoryMatch(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ���ʷ�ɽ���¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryMatch(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryHistoryMatch(cmd, state);
}

/**
* @brief ��ѯ�ͻ����ճ�����¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryTodayCash(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ����ճ�����¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayCash(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryTodayCash(cmd, state);
}

/**
* @brief ��ѯ�ͻ���ʷ������¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryHistoryCash(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ���ʷ������¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryCash(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryHistoryCash(cmd, state);
}

/**
* @brief ��ѯ�ͻ��ʽ��յ�����¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryTodayCashAdjust(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ��ʽ��յ�����¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryTodayCashAdjust(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryTodayCashAdjust(cmd, state);
}

/**
* @brief ��ѯ�ͻ��ʽ���ʷ������¼��ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnQueryHistoryCashAdjust(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��ѯ�ͻ��ʽ���ʷ������¼
* @param {PNetCommand} cmd �������
* @return ��
*/
void QueryHistoryCashAdjust(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnQueryHistoryCashAdjust(cmd, state);
}

/**
* @brief ����������ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnOrderInsert(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��������
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderInsert(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnOrderInsert(cmd, state);
}

/**
* @brief �ĵ�������ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnOrderModify(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief �ĵ�����
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderModify(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnOrderModify(cmd, state);
}

/**
* @brief ����������ɵ���Ϣ�ط�
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
void OnOrderCancel(const PNetCommand cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = 0 - state;
	server_message_send(result);
}

/**
* @brief ��������
* @param {PNetCommand} cmd �������
* @return ��
*/
void OrderCancel(const PNetCommand cmd)
{
	COMMAND_STATE state = NET_COMMAND_STATE_SUCCEED;
	try
	{
		//deserialize_cmd_arg(cmd,DATA_TYPE,cmd_arg);//��������Ϊcmd_arg
		//TO DO:������
	}
	catch (...)
	{
		state = 65536;
	}
	OnOrderCancel(cmd, state);
}
#endif