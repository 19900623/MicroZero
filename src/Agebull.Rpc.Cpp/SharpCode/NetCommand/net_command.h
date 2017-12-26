#ifndef _AGEBULL_NET_COMMAND_H
#define _AGEBULL_NET_COMMAND_H
#pragma once
#include "stdinc.h"
#ifdef NOCLR
#include "redis/redis.h"
#endif
#ifdef WIN32
#include <objbase.h>  
#else
#include <uuid/uuid.h>  
typedef uuid_t GUID;
#endif

#include "command_def.h"
#include <tson/tson_serializer.h>
#include <Tson/tson_deserializer.h>
#ifdef EsTrade
#define COMMANDPROXY
#endif
#ifdef GBS_TRADE
#define COMMANDPROXY
#endif

using namespace std;
//�ͻ����첽��Ϣ����
void notify_sub(const char *address);
//�ͻ�����������
void request_cmd(const char *address);
//��ʼ�����������
int init_net_command();
//�������������
void distory_net_command();
//�ر����������
void close_net_command();
//�Ǽ��߳̿�ʼ
void set_command_thread_start();
//�Ǽ��̹߳ر�
void set_command_thread_end();
#ifdef NOCLR
//�������������
int start_net_command();
#else
//�������������
int start_net_command(const char* pub_addr, const char* cmd_addr, const char* quote_addr);
#endif
#ifdef NOCLR
//point�Ƿ�������ʶ(���ؽ���:lt, ��ʢ���� : et, ��ʢ���� : eq, �������� : lq, �������˺� : la, �������� : lo)
#ifdef EsTrade
#define command_key_fmt "b:cmd:et:%s"
#else
#define command_key_fmt "b:cmd:lt:%s"
#endif

//������
DWORD zmq_monitor(const char * address);

#ifdef SERVER
//�����·�ɣ�δʵ�֣�
//void server_route();
//����������
void server_request();
//������첽�ط�
void server_answer();
//�������Ϣ�㲥
void server_notify();
//�������Ϣ��
void server_message_pump();
//�������Ϣ�ط�
void command_answer(NetCommandArgPtr& cmd);
//�������Ϣ����
void command_answer(PNetCommand cmd);

//����˹㲥����
void notify_message(PNetCommand cmd);
//����˹㲥����
void notify_message(NetCommandArgPtr& cmd);

//�����ִ���������
void push_cmd(NetCommandArgPtr& call);
//�����ִ���������
void push_cmd(PNetCommand arg);
/**
* @brief ��������״̬
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
inline void publish_command_state(NetCommandArgPtr& cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	memcpy(result, cmd.m_command, sizeof(NetCommand));
	result->cmd_state = state;
	result->data_len = 0;
	NetCommandArgPtr ptr(result);
	command_answer(ptr);
}/**
* @brief ��������״̬
* @param {PNetCommand} cmd �������
* @param {COMMAND_STATE} state ִ��״̬
* @return ��
*/
inline void publish_command_state(NetCommand* cmd, COMMAND_STATE state)
{
	PNetCommand result = new NetCommand();
	//memset(&result, 0, sizeof(NetCommandResult));
	memcpy(result, cmd, sizeof(NetCommand));
	result->cmd_state = state;
	result->data_len = 0;
	NetCommandArgPtr ptr(result);
	command_answer(ptr);
}

//�������ݱ�����û�
template<class T>
inline void send_data_to_user(T& data, const char* token)
{
	if (token == nullptr)
		return;
	PNetCommand cmd_arg = SerializeToCommand(&data);
	size_t len = strlen(token);
	if (len > GUID_LEN)
		len = GUID_LEN - 1;
	for (size_t idx = 0; idx < len; idx++)
	{
		cmd_arg->user_token[idx] = token[idx];
	}
	cmd_arg->cmd_id = NET_COMMAND_DATA_CHANGED;
	cmd_arg->cmd_state = NET_COMMAND_STATE_DATA;
	command_answer(cmd_arg);
}

//�������ݱ�����û�
template<class T>
inline void send_data_to_user(T& data, const char* token, const char* cmdkey)
{
	if (token == nullptr)
		return;
	PNetCommand cmd_arg = SerializeToCommand(&data);

	size_t len = strlen(token);
	if (len > GUID_LEN)
		len = GUID_LEN - 1;
	for (size_t idx = 0; idx < len; idx++)
	{
		cmd_arg->user_token[idx] = token[idx];
	}
	len = strlen(cmdkey);
	if (len > GUID_LEN)
		len = GUID_LEN - 1;
	for (size_t idx = 0; idx < len; idx++)
	{
		cmd_arg->cmd_identity[idx] = cmdkey[idx];
	}
	cmd_arg->cmd_id = NET_COMMAND_DATA_CHANGED;
	cmd_arg->cmd_state = NET_COMMAND_STATE_DATA;
	command_answer(cmd_arg);
}
//�����û������������
template<class T>
inline void send_cmd_data(T& data, PNetCommand call_arg)
{
	PNetCommand cmd_arg = SerializeToCommand(&data);
	memcpy(cmd_arg, call_arg, NETCOMMAND_HEAD_LEN);
	cmd_arg->cmd_id = NET_COMMAND_DATA_PUSH;
	cmd_arg->cmd_state = NET_COMMAND_STATE_DATA;
	command_answer(cmd_arg);
}
//����֪ͨ����
template<class T>
inline void notify_data(T& data, const char* token)
{
	if (token == nullptr)
		return;
	PNetCommand cmd_arg = SerializeToCommand(&data);
	size_t len = strlen(token);
	if (len > GUID_LEN)
		len = GUID_LEN - 1;
	for (size_t idx = 0; idx < len; idx++)
	{
		cmd_arg->user_token[idx] = token[idx];
	}
	cmd_arg->cmd_id = NET_COMMAND_DATA_CHANGED;
	cmd_arg->cmd_state = NET_COMMAND_STATE_DATA;
	notify_message(cmd_arg);
}
#endif
#endif
#ifdef CLIENT_COMMAND

//����GUID
inline GUID create_guid()
{
	GUID guid;
	CoCreateGuid(&guid);
	return guid;
}
//GUID��ӡ���ı�
inline void print_guid(GUID& guid, char* buffer)
{
	sprintf_s(buffer, 33, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}
//��ʼ���ͻ���
void init_client();
//����ͻ���
void distory_client();
//�ͻ����û���ʶ
char* get_user_token();
//��������ͷ
void set_command_head(PNetCommand arg, NET_COMMAND cmd);
//�ͻ��������
DWORD WINAPI client_cmd(LPVOID);
//�ͻ����첽��Ϣ����
DWORD WINAPI client_sub(LPVOID);
//�ͻ����첽��Ϣ����
DWORD WINAPI quote_sub(LPVOID arg);
//�ͻ�����Ϣ��
DWORD WINAPI client_message_pump(LPVOID);
//������
DWORD WINAPI zmq_monitor(LPVOID arg);
//�����������
DWORD WINAPI quote_pump(LPVOID arg);

//���鶩��
DWORD quote_notify_sub(const char *address);
//�������
void request_net_cmmmand(NetCommandArgPtr& arg);
//�������
inline void request_net_cmmmand(PNetCommand arg, void*)
{
	NetCommandArgPtr ptr(arg);
	request_net_cmmmand(ptr);
}
//�������
inline void request_net_cmmmand(PNetCommand arg)
{
	NetCommandArgPtr ptr(arg);
	request_net_cmmmand(ptr);
}
#endif
#ifdef INNER_SERVER
//��ʼ���ͻ���
void init_client();
//����ͻ���
void distory_client();
//������
void proxy_sub();
//��������
void proxy_cmd();
//������Ϣ
void proxy_message_pump();
//��������ͷ
void set_command_head(PNetCommand arg, NET_COMMAND cmd);

//�������
void request_net_cmmmand(NetCommandArgPtr& arg);
//�������
inline void request_net_cmmmand(PNetCommand arg, void*)
{
	NetCommandArgPtr ptr(arg);
	request_net_cmmmand(ptr);
}
//�������
inline void request_net_cmmmand(PNetCommand arg)
{
	NetCommandArgPtr ptr(arg);
	request_net_cmmmand(ptr);
}
#endif
#ifdef INNER_SETTLEMENT
//���鶩��
int quote_sub();
//���鶩��
DWORD quote_notify_sub(const char *address);
//���鶩��
void quote_pump();
//�������ݱ�������׷���
template<class T>
inline void send_change_to_trade(T data)
{
	PNetCommand cmd_arg = SerializeToCommand(&data);
	set_command_head(cmd_arg, NET_COMMAND_DATA_CHANGED);
	request_net_cmmmand(cmd_arg);
}
#endif
//֪ͨ��Ϣ��
void notify_message_pump();
//ZMQ�����Ķ���
ZMQ_HANDLE get_zmq_context();
//����״̬
NET_STATE get_net_state();
//д��CRCУ����
void write_crc(PNetCommand cmd);
//У��CRCУ����
bool check_crc(PNetCommand cmd);

//�����л���
#define deserialize_cmd_arg(cmd,type,name) \
	type name;\
	Agebull::Tson::Deserializer reader(get_cmd_buffer(cmd));\
	Deserialize(reader, &name);


//���л���
#define serialize_to_cmd(type,args,cmd_num) \
	PNetCommand net_cmd = reinterpret_cast<PNetCommand>(new char[sizeof(type) * 2]);\
	memset(net_cmd,0,NETCOMMAND_HEAD_LEN);\
	net_cmd->cmd_id = cmd_num;\
	Agebull::Tson::Serializer wirter(get_cmd_buffer(net_cmd), sizeof(type) * 2 - NETCOMMAND_HEAD_LEN);\
	Serialize(wirter, &args);


#endif