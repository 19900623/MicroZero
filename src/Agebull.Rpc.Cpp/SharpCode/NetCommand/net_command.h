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
//��ʼ�����������
int init_net_command();
//�������������
int start_net_command();
//�������������
void distory_net_command();
//�ر����������
void close_net_command();
//�Ǽ��߳̿�ʼ
void set_command_thread_start();
//�Ǽ��̹߳ر�
void set_command_thread_end();

//������
DWORD zmq_monitor(const char * address);


//ZMQ�����Ķ���
ZMQ_HANDLE get_zmq_context();
//����״̬
NET_STATE get_net_state();
//д��CRCУ����
void write_crc(PNetCommand cmd);
//У��CRCУ����
bool check_crc(PNetCommand cmd);

//point�Ƿ�������ʶ(���ؽ���:lt, ��ʢ���� : et, ��ʢ���� : eq, �������� : lq, �������˺� : la, �������� : lo)
#define command_key_fmt "b:cmd:lt:%s"

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



//����ZMQ���Ӷ���
inline static ZMQ_HANDLE create_socket(int type, const char* address)
{
	ZMQ_HANDLE socket = zmq_socket(get_zmq_context(), type);
	if (socket == nullptr)
	{
		auto err = zmq_strerror(errno);
		log_error2("����SOCKET����(%s)��������:%s", address, err);
		return nullptr;
	}
	//zmq_result = zmq_socket_monitor(socket, "inproc://server_cmd.rep", ZMQ_EVENT_ALL);
	//assert(zmq_result == 0);
	int iZMQ_IMMEDIATE = 1;//����Ϣֻ����������ɵ�����
	zmq_setsockopt(socket, ZMQ_LINGER, &iZMQ_IMMEDIATE, sizeof(int));
	int iLINGER = 50;//�ر�����ͣ��ʱ��,����
	zmq_setsockopt(socket, ZMQ_LINGER, &iLINGER, sizeof(int));
	int iRcvTimeout = 500;
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(int));
	int iSndTimeout = 500;
	zmq_setsockopt(socket, ZMQ_SNDTIMEO, &iSndTimeout, sizeof(int));
	int zmq_result = zmq_bind(socket, address);
	if (zmq_result < 0)
	{
		auto err = zmq_strerror(errno);
		zmq_close(socket);
		log_error2("�󶨶˿�(%s)��������:%s", address, err);
		return nullptr;
	}
	return socket;
}
//���ZMQ����
inline static bool check_zmq_error(int& state, const char* address, const char* action)
{
	switch (errno)
	{
	case ETERM:
		log_error2("%s(%s)����[��ָ����socket�������context���ս���],�Զ��ر�", action, address);
		state = 1;
		return false;
	case ENOTSOCK:
		log_error2("%s(%s)����[ָ����socket������],�Զ�����", action, address);
		state = 2;
		return false;
	case EINTR:
		log_error2("%s(%s)����[�ڽӽ��յ���Ϣ֮ǰ�����������ϵͳ�ź��ж���],�Զ�����", action, address);
		state = 2;
		return false;
	case EAGAIN://ʹ�÷�������ʽ������Ϣ��ʱ��û�н��յ��κ���Ϣ��
	default:
		state = 0;
		return true;
	}
}
#endif
