#ifndef _AGEBULL_NET_COMMAND_H
#define _AGEBULL_NET_COMMAND_H
#pragma once
#include "stdinc.h"
#include "command_def.h"

using namespace std;
//��ʼ�����������
int init_net_command();
//�������������
int start_net_command();
//�������������
void distory_net_command();
//�ر����������
void close_net_command(bool wait=true);
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
//����״̬
void set_net_state(NET_STATE state);
//д��CRCУ����
void write_crc(PNetCommand cmd);
//У��CRCУ����
bool check_crc(PNetCommand cmd);
#endif
