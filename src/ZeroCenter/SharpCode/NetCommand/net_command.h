#ifndef _AGEBULL_NET_COMMAND_H
#define _AGEBULL_NET_COMMAND_H
#pragma once
#include "stdinc.h"
namespace agebull
{
	typedef void* ZMQ_HANDLE;

	typedef char* TSON_BUFFER;

	using namespace std;

	//����״̬
	typedef int NET_STATE;
	const NET_STATE NET_STATE_NONE = 0;
	const NET_STATE NET_STATE_RUNING = 1;
	const NET_STATE NET_STATE_CLOSING = 2;
	const NET_STATE NET_STATE_CLOSED = 3;
	const NET_STATE NET_STATE_DISTORY = 4;

	//ZMQ�����Ķ���
	ZMQ_HANDLE get_zmq_context();
	//����״̬
	NET_STATE get_net_state();
	//����״̬
	//void set_net_state(NET_STATE state);

	//��ʼ�����������
	int init_net_command();
	//�������������
	int start_net_command();
	//�������������
	void distory_net_command();
	//�ر����������
	void close_net_command(bool wait = true);

	//�̼߳�������
	void reset_command_thread();
	//�Ǽ��߳�ʧ��
	void set_command_thread_bad(const char* name);
	//�Ǽ��߳̿�ʼ
	void set_command_thread_start(const char* name);
	//�Ǽ��̹߳ر�
	void set_command_thread_end(const char* name);

	//������
	DWORD zmq_monitor(const char * address);
	//����CRCУ����
	ushort get_crc(const char *msg, size_t len);
}
#endif
