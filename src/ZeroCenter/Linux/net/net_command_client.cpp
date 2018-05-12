#include <stdafx.h>
#include "entity.h"
#include "debug/TraceStack.h"
#ifdef  WEB
#define USER_TOKEN "$WA"
#define CMD_TOKEN "@WA"
#define INNER_CLIENT
#endif

using namespace std;


//C�˵�������ö���
queue<NetCommandArgPtr> call_queue;
///C��������ö�����
HANDLE call_mutex;

//C�˵�������Ϣ����
std::queue<NetCommandArgPtr> back_queue;
//C������Ϣ������
HANDLE back_mutex;
//������Ϣ����
std::queue<NetCommandArgPtr> quote_queue;
//C������Ϣ������
HANDLE quote_mutex;

#define front_queue(cmd_msg,queue,mutex)\
	if (WaitForSingleObject(mutex, 100) == WAIT_TIMEOUT)\
	{\
		continue;\
    }\
	if (queue.empty())\
	{\
		ReleaseMutex(mutex);\
		thread_sleep(1);\
		continue;\
	}\
	cmd_msg = queue.front();\
	queue.pop();\
	ReleaseMutex(mutex)

#define push_queue(cmd_arg,queue,mutex)\
	WaitForSingleObject(mutex, INFINITE);\
	queue.push(NetCommandArgPtr(cmd_arg));\
	ReleaseMutex(mutex)

#ifdef INNER_SERVER
//��������ͷ
void set_command_head(PNetCommand call_arg, NET_COMMAND cmd)
{
	call_arg->cmd_id = cmd;
}
#endif
#ifdef CLIENT_COMMAND
//�ͻ����û���ʶ
char request_user_token[GUID_LEN];
//�ͻ����û���ʶ
char* get_user_token()
{
	return request_user_token;
}
//��������ͷ
void set_command_head(PNetCommand arg, NET_COMMAND cmd)
{
	memset(arg, 0, NETCOMMAND_HEAD_LEN);
	arg->cmd_id = cmd;
	//�����û�ͷ
	strcpy_s(arg->user_token, request_user_token);
	//���������ʶ
	arg->cmd_identity[0] = '*';
	GUID cmd_key = create_guid();
	print_guid(cmd_key, arg->cmd_identity + 1);
}
DWORD WINAPI client_cmd(LPVOID arg)
{
	if (get_net_state() != NET_STATE_RUNING)
		return 1;
	request_cmd(static_cast<char*>(arg));
	return 0;
}

DWORD WINAPI client_sub(LPVOID arg)
{
	notify_sub(static_cast<char*>(arg));
	return 0;
}
DWORD WINAPI quote_sub(LPVOID arg)
{
	quote_notify_sub(static_cast<char*>(arg));
	return 0;
}
//�ͻ�����Ϣ��
DWORD WINAPI client_message_pump(LPVOID)
{
	log_msg("�ͻ�����Ϣ��������");
	notify_message_pump();
	log_msg("�ͻ�����Ϣ���ѹر�");
	return 0;
}
#endif
//��ʼ���ͻ���
void init_client()
{
	//�ͻ����û���ʶ
#ifdef CLIENT_COMMAND
	GUID guid = create_guid();
	print_guid(guid, request_user_token + 1);
	request_user_token[0] = '$';
	log_msg1("�û�����%s",request_user_token);
#endif
	call_mutex = CreateMutex(nullptr, FALSE, nullptr);
	back_mutex = CreateMutex(nullptr, FALSE, nullptr);
	quote_mutex = CreateMutex(nullptr, FALSE, nullptr);
}
//����ͻ���
void distory_client()
{
	CloseHandle(call_mutex);
	CloseHandle(back_mutex);
	CloseHandle(quote_mutex);
}
#ifdef INNER_SETTLEMENT
#pragma warning(disable: 4996)
int quote_sub()
{
	int cnt = 0;
	string& cfg = config::get_config("quote_addr");
	std::vector<std::string> address;
	boost::split(address, cfg, boost::is_any_of(_T(",")));
	for (string& addr : address)
	{
		size_t len = addr.length() + 1;
		char* ca = new char[len];
		strcpy_s(ca, len, addr.c_str());

		boost::thread thrds_r1(boost::bind(&quote_notify_sub, ca));
		cnt++;
	}
	return cnt;
}
#pragma warning(default: 4996)
#endif
#ifdef INNER_SERVER
void proxy_sub()
{
	notify_sub(config::get_config("px_sub_addr").c_str());
}

void proxy_cmd()
{
	request_cmd(config::get_config("px_cmd_addr").c_str());
}

void proxy_message_pump()
{
	notify_message_pump();
}
#endif
//�������
void request_net_cmmmand(NetCommandArgPtr& arg)
{
#ifndef CLIENT_COMMAND
	if (arg->cmd_id == 0)
		arg->cmd_id = NET_COMMAND_BUSINESS_NOTIFY;
#endif
	if (arg.m_command->cmd_state == NET_COMMAND_STATE_SERVER_UNKNOW)
	{
		log_error2("��Ϣ�����쳣(�������%d)\r\n%s", arg.m_command->cmd_state, get_call_stack());
	}
	else if (arg.m_command->cmd_state == NET_COMMAND_STATE_DATA && (arg.m_command->data_len <= 0 || arg.m_command->data_len > 0x4000))
	{
		log_error1("��Ϣ�����쳣(���ݷ���Ϊ��)\r\n%s", get_call_stack());
	}
	else if (arg.m_command->cmd_state != NET_COMMAND_STATE_DATA && arg.m_command->data_len != 0)
	{
		log_error1("��Ϣ�����쳣(״̬����ȴ��������)\r\n%s", get_call_stack());
	}
	push_queue(arg, call_queue, call_mutex);
}

//���ͳ�������Դ���
void on_retry(const char* address, NetCommandArgPtr call_arg, const char* state)
{
	if (call_arg->try_num >= 5)
	{
		call_arg->cmd_state = NET_COMMAND_STATE_NETERROR;
		push_queue(call_arg, back_queue, back_mutex);
		log_error4("%s:����%d(%s)%s,�����Դ���̫��(5��),����ֹ����", 
			address, 
			call_arg->cmd_id, 
			call_arg->cmd_identity, 
			state);
	}
	else
	{
		call_arg->cmd_state = NET_COMMAND_STATE_UNKNOW;
		push_queue(call_arg, call_queue, call_mutex);
		log_error5("%s:����%d(%s)%s,�����Ŷ��ط�(%d)", 
			address, 
			call_arg->cmd_id, 
			call_arg->cmd_identity,
			state,
			call_arg->try_num);
	}
}

void request_cmd(const char* address)
{
	log_msg1("���������(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_REQ);
	if (socket == nullptr)
	{
		log_error2("���������(%s),����SOCKET����������:%s", address, zmq_strerror(errno));
		return;
	}
	int zmq_result/* = zmq_socket_monitor(socket, "inproc://request_cmd.rep", ZMQ_EVENT_ALL)*/;
	//assert(zmq_result == 0);
	zmq_result = zmq_connect(socket, address);
	if (zmq_result < 0)
	{
		log_error2("���������(%s)���ӷ�������:%s", address, zmq_strerror(errno));
		return;
	}
#ifdef CLIENT_COMMAND
	zmq_setsockopt(socket, ZMQ_IDENTITY, request_user_token, strlen(request_user_token));
#endif
	int iZMQ_IMMEDIATE = 1;//����Ϣֻ����������ɵ�����
	zmq_setsockopt(socket, ZMQ_LINGER, &iZMQ_IMMEDIATE, sizeof(int));
	int iLINGER = 50;//�ر�����ͣ��ʱ��,����
	zmq_setsockopt(socket, ZMQ_LINGER, &iLINGER, sizeof(int));
	int iRcvTimeout = 500;
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(int));
	int iSndTimeout = 500;
	zmq_setsockopt(socket, ZMQ_SNDTIMEO, &iSndTimeout, sizeof(int));

	log_msg1("���������(%s)������", address);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	int state = 0;
	while (get_net_state() == NET_STATE_RUNING)
	{
		if (call_queue.empty())
		{
			thread_sleep(1);
			continue;
		}
		NetCommandArgPtr call_arg;
		front_queue(call_arg, call_queue, call_mutex);
#ifdef INNER_CLIENT
		for (int idx = 0;idx < GUID_LEN;idx++)
		{
			call_arg->user_token[0] = '\0';
			call_arg->cmd_identity[0] = '\0';
		}
		strcpy_s(call_arg->user_token, USER_TOKEN);
		strcpy_s(call_arg->cmd_identity, CMD_TOKEN);
#endif
		call_arg->try_num += 1;
		write_crc(call_arg.m_command);

		int len = get_cmd_len(call_arg);
		//������������
		zmq_msg_t msg_call;
		zmq_result = zmq_msg_init(&msg_call);
		if (zmq_result != 0)
		{
			state = 2;//������
			break;
		}
		zmq_result = zmq_msg_init_data(&msg_call, call_arg.m_buffer, len, nullptr, nullptr);
		if (zmq_result != 0)
		{
			state = 2;//������
			break;
		}
		zmq_result = zmq_msg_send(&msg_call, socket, 0);
		zmq_msg_close(&msg_call);
		if (zmq_result < 0)
		{
			switch (errno)
			{
			case ETERM:
				log_error2("����%d(%s)���ʹ���[��ָ����socket�������context���ս���],�Զ��ر�", call_arg->cmd_id, call_arg->cmd_identity);
				state = 1;
				break;
			case ENOTSOCK:
				log_error2("����%d(%s)���ʹ���[ָ����socket������],�Զ�����", call_arg->cmd_id, call_arg->cmd_identity);
				state = 2;
				break;
			case EINTR:
				log_error2("����%d(%s)���ʹ���[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", call_arg->cmd_id, call_arg->cmd_identity);
				state = 2;
				break;
			case EAGAIN:
				//ʹ�÷�������ʽ������Ϣ��ʱ��û�н��յ��κ���Ϣ��
			default:
				state = 0;
				on_retry(address, call_arg, "����ʧ��");
				break;
			}
			if (state > 0)
				break;
			continue;
		}
		log_debug4(DEBUG_CALL, 3, "%s:����%d(%s)���ͳɹ�(%d)", address, call_arg->cmd_id, call_arg->cmd_identity, zmq_result);
		call_arg->cmd_state = NET_COMMAND_STATE_SENDED;
		//���մ�����
		char result[10];
		zmq_result = zmq_recv(socket, result, 10, 0);
		if (zmq_result < 0)
		{
			switch (errno)
			{
			case ETERM:
				log_error2("����%d(%s)���ջ�ִ����[��ָ����socket�������context���ս���],�Զ��ر�", call_arg->cmd_id, call_arg->cmd_identity);
				state = 1;
				break;
			case ENOTSOCK:
				log_error2("����%d(%s)���ջ�ִ����[ָ����socket������],�Զ�����", call_arg->cmd_id, call_arg->cmd_identity);
				state = 2;
				break;
			case EINTR:
				log_error2("����%d(%s)���ջ�ִ����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", call_arg->cmd_id, call_arg->cmd_identity);
				state = 2;
				break;
			case EAGAIN://ʹ�÷�������ʽ������Ϣ��ʱ��û�н��յ��κ���Ϣ��
			default:
				state = 2;
				break;
			}
			on_retry(address, call_arg, "δ�յ���ִ");
			break;
		}
		if (result[0] == '0')
		{
			if (get_net_state() != NET_STATE_RUNING)
			{
				zmq_msg_close(&msg_call);
				break;
			}
			on_retry(address, call_arg, "�յ������ִ");
		}
		else
		{
			log_debug3(DEBUG_CALL, 3, "%s:����%d(%s)���յ���ȷ��ִ", address, call_arg->cmd_id, call_arg->cmd_identity);
		}
		zmq_msg_close(&msg_call);
	}
	try
	{
		zmq_disconnect(socket, address);
	}
	catch (const std::exception& ex)
	{
		log_error1("%s:����ر��쳣", address);
	}
	zmq_close(socket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
	if (state == 2 && get_net_state() == NET_STATE_RUNING)
	{
		log_msg("�ͻ����������������");
#ifdef CLIENT_COMMAND
		start_thread(client_cmd, LPVOID(address));
#else
		boost::thread thrds(boost::bind(&proxy_cmd));
#endif
	}
	else
	{
		log_msg("�ͻ���������ѹر�");
	}
}

DWORD quote_notify_sub(const char* address)
{
	log_msg1("���鶩��(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_SUB);

	if (socket == nullptr)
	{
		log_error2("���鶩��(%s),����SOCKET����������:%s", address, zmq_strerror(errno));
		return 0;
	}
	int zmq_result = zmq_connect(socket, address);
	if (zmq_result < 0)
	{
		log_error2("���鶩��(%s)���ӷ�������:%s", address, zmq_strerror(errno));
		return 0;
	}
	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, nullptr, 0);
	int iRcvTimeout = 500;
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout));
	log_msg1("���鶩��(%s)������", address);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	while (get_net_state() == NET_STATE_RUNING)
	{
		zmq_msg_t msg_sub;
		zmq_result = zmq_msg_init(&msg_sub);
		assert(zmq_result == 0);
		zmq_result = zmq_msg_recv(&msg_sub, socket, 0);
		if (zmq_result <= 0)
		{
			zmq_msg_close(&msg_sub);
			thread_sleep(1);
			int error = errno;
			if (error != 0)
				log_error2("�������鶩��(%s)ʱ��������(%s)", address, zmq_strerror(errno));
			continue;
		}
		char* msg = static_cast<char*>(zmq_msg_data(&msg_sub));

		if (!check_crc(reinterpret_cast<PNetCommand>(msg)))// (msg[len - 1] != '#')
		{
			log_error1("�������鶩��(%s)ʱ��������:���ݲ���ȷ", address);
			zmq_msg_close(&msg_sub);
			continue;
		}
#ifdef CLIENT_TEST
		PNetCommand cmd_msg = static_cast<PNetCommand>(zmq_msg_data(&msg_sub));
		void* data = Agebull::Futures::DataModel::DataFactory::get_command_data(cmd_msg);
		delete data;
#endif 
#if (defined INNER_SETTLEMENT) || (defined CLIENT_COMMAND)
		size_t len = zmq_msg_size(&msg_sub);
		char* buf = new char[len];
		memcpy(buf, msg, len);
		push_queue(reinterpret_cast<PNetCommand>(buf), quote_queue, quote_mutex);
#endif
		zmq_msg_close(&msg_sub);
	}
	zmq_disconnect(socket, address);
	log_msg1("���鶩��(%s)�ѹر�!", address);
	zmq_close(socket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
	return 1;
}

#ifdef INNER_SETTLEMENT
void quote_pump()
{
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, quote_queue, quote_mutex);
#ifdef RISK_CONTROL
		Agebull::Futures::RiskControl::RiskControler::check_quote(reinterpret_cast<PNetCommand>(cmd_msg.m_command));
#endif
#ifdef TRADE_SETTLEMENT
		Agebull::Futures::TradeSettlement::RealSettlement::check_quote(reinterpret_cast<PNetCommand>(cmd_msg.m_command));
#endif
	}
}
#endif
void notify_sub(const char* address)
{
	log_msg1("�ͻ�����Ϣ����(%s)��������", address);
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), ZMQ_SUB);
	if (socket == nullptr)
	{
		log_error2("�ͻ�����Ϣ����(%s),����SOCKET����������:%s", address, zmq_strerror(errno));
		return;
	}
	int zmq_result/* = zmq_socket_monitor(socket, "inproc://notify_sub.rep", ZMQ_EVENT_ALL);
	assert(zmq_result == 0)*/;
	zmq_result = zmq_connect(socket, address);
	if (zmq_result < 0)
	{
		log_error2("�ͻ�����Ϣ����(%s)���ӷ�������:%s", address, zmq_strerror(errno));
		return;
	}
#ifdef CLIENT
	log_msg2("�ͻ�����Ϣ����(%s)������Ϊ(%s)", address, request_user_token);
	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, request_user_token, strlen(request_user_token));
#else
	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, nullptr, 0);
#endif
#ifdef CLIENT_TEST
	Agebull::Futures::TradeManagement::TestNotify notify;
#endif
	int iRcvTimeout = 500;
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout));
	log_msg1("�ͻ�����Ϣ����(%s)������", address);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	while (get_net_state() == NET_STATE_RUNING)
	{
		zmq_msg_t msg_sub;
		zmq_result = zmq_msg_init(&msg_sub);
		assert(zmq_result == 0);
		zmq_result = zmq_msg_recv(&msg_sub, socket, 0);
		if (zmq_result <= 0)
		{
			zmq_msg_close(&msg_sub);
			thread_sleep(1);
			int error = errno;
			if (error != 0)
				log_error2("���ն���(%s)ʱ��������(%s)", address, zmq_strerror(errno));
			continue;
		}
		size_t len = zmq_msg_size(&msg_sub);
		PNetCommand cmd_msg = static_cast<PNetCommand>(zmq_msg_data(&msg_sub));
		if (!check_crc(cmd_msg))// (cmd_msg->user_token[0] != '$')
		{
			zmq_msg_close(&msg_sub);
			log_error("�յ���������֡");
			continue;
		}
#ifdef CLIENT_TEST
		notify.message_pump(cmd_msg);
#else
#ifdef WEB
		//�Լ�����
		if (cmd_msg->cmd_identity[0] == '@' && cmd_msg->cmd_identity[1] == 'W' && cmd_msg->cmd_identity[2] == 'A' && cmd_msg->cmd_identity[3] == '\0')
		{
			zmq_msg_close(&msg_sub);
			continue;
		}
		//if (cmd_msg->cmd_id <= NET_COMMAND_USER_SET_PASSWORD || cmd_msg->cmd_id == NET_COMMAND_DATA_PUSH)
		//{
		//	zmq_msg_close(&msg_sub);
		//	continue;
		//}
#endif
#ifdef INNER_SETTLEMENT
		//��������������Ϊ���͸��ͻ���
		if (cmd_msg->cmd_id < NET_COMMAND_DATA_CHANGED || strcmp(cmd_msg->cmd_identity, CMD_TOKEN) == 0)
		{
			zmq_msg_close(&msg_sub);
			continue;
		}
#else
#ifdef _DEBUG
		log_trace3(DEBUG_RESULT, 3, "���յ�����(%s)������(%d)���ص�״̬(%d)", address, cmd_msg->cmd_id, cmd_msg->cmd_state);
#endif
#endif
		char* buf = new char[len];
		memcpy(buf, zmq_msg_data(&msg_sub), len);
		push_queue(reinterpret_cast<PNetCommand>(buf), back_queue, back_mutex);
#endif
		zmq_msg_close(&msg_sub);
	}
	zmq_disconnect(socket, address);
	log_msg1("�ͻ�����Ϣ����(%s)�ѹر�!", address);
	zmq_close(socket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
}

#ifdef CLR
#pragma managed
DWORD WINAPI quote_pump(LPVOID arg)
{
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, quote_queue, quote_mutex);
		Agebull::Futures::Globals::Client::CommandProxy::get_Single()->FireQuote(cmd_msg.m_command);
	}
	return 1;
}
void notify_message_pump()
{
	log_msg("֪ͨ��Ϣ��������");
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, back_queue, back_mutex);
		Agebull::Futures::Globals::Client::CommandProxy::get_Single()->FireEvents(cmd_msg);
	}
	log_msg("֪ͨ��Ϣ���ѹر�");
}
#endif

#ifdef GBS_TRADE
void notify_message_pump()
{
	log_msg("֪ͨ��Ϣ��������");
	/*Agebull::Futures::TradeManagement::EsNotify notify;
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, back_queue, back_mutex);
		RedisTransScope scope;
		notify.message_pump(cmd_msg);
	}*/
	log_msg("֪ͨ��Ϣ���ѹر�");
}
#endif
#ifdef RISK_CONTROL
void notify_message_pump()
{
	log_msg("֪ͨ��Ϣ��������");
	Agebull::Futures::RiskControl::RiskControler notify;
	notify.Initialize();
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, back_queue, back_mutex);
		RedisTransScope scope;
		notify.message_pump(cmd_msg);
	}
	log_msg("֪ͨ��Ϣ���ѹر�");
}
#endif
#ifdef TRADE_SETTLEMENT
void notify_message_pump()
{
	log_msg("֪ͨ��Ϣ��������");
	Agebull::Futures::TradeSettlement::RealSettlement notify;
	notify.Initialize();
	while (get_net_state() != NET_STATE_DISTORY)
	{
		NetCommandArgPtr cmd_msg;
		front_queue(cmd_msg, back_queue, back_mutex);
		RedisTransScope scope;
		notify.message_pump(cmd_msg);
	}
	log_msg("֪ͨ��Ϣ���ѹر�");
}
#endif