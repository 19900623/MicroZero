#include "stdafx.h"
#include "net_command.h"
#include "BroadcastingStation.h"
#include "NetDispatcher.h"
using namespace std;

ZMQ_HANDLE net_context;
volatile NET_STATE net_state = NET_STATE_NONE;
//��ǰ�����˶��������߳�
volatile int zero_thread_count=0;
//��ǰ�����߳�δ��������
volatile int zero_thread_bad=0;
//ZMQ�����Ķ���
ZMQ_HANDLE get_zmq_context()
{
	return net_context;
}

//�̼߳�������
void reset_command_thread()
{
	zero_thread_bad = 0;
	zero_thread_count = 0;
}
//�Ǽ��߳̿�ʼ
void set_command_thread_start(const char* name)
{
	zero_thread_count++;
	log_msg2("zero thread join ��%s��(%d)", name, zero_thread_count);
}
//�Ǽ��߳�ʧ��
void set_command_thread_bad(const char* name)
{
	zero_thread_bad++;
	log_msg2("zero thread bad ��%s��(%d)", name, zero_thread_bad);
}
//�Ǽ��̹߳ر�
void set_command_thread_end(const char* name)
{
	zero_thread_count--;
	log_msg2("zero thread left ��%s��(%d)", name, zero_thread_count);
}
//����״̬
NET_STATE get_net_state()
{
	return net_state;
}
//����״̬
//void set_net_state(NET_STATE state)
//{
//	net_state = state;
//	log_msg1("zero state changed(%d)", net_state);
//}
//#ifdef COMMANDPROXY
//CommandProxy* proxy = new CommandProxy();
//#endif
//��ʼ�����������
int init_net_command()
{
	log_msg("zero center initiate...");
	net_state = NET_STATE_NONE;
	net_context = zmq_ctx_new();

	assert(net_context != nullptr);

	zmq_ctx_set(net_context, ZMQ_MAX_SOCKETS, 8192);
	//zmq_ctx_set(net_context, ZMQ_IO_THREADS, 16);
	//zmq_ctx_set(net_context, ZMQ_MAX_MSGSZ, 8192);


	//boost::thread smp(boost::bind(&server_message_pump));

	//thread_sleep(50);
	log_msg("zero center initiated");
	return net_state;
}
//�������������
int start_net_command()
{
	net_state = NET_STATE_RUNING;

	log_msg("start zero monitor");
	agebull::zmq_net::SystemMonitorStation::run();
	while (zero_thread_count < 1)
	{
		cout << ".";
		thread_sleep(50);
	}
	cout << endl;
	agebull::zmq_net::monitor("*", "system_start", "*************Wecome ZeroNet,luck erery day!*************");
	log_msg("start zero command");
	agebull::zmq_net::NetDispatcher::run();
	while (zero_thread_count < 2)
	{
		cout << ".";
		thread_sleep(50);
	}
	cout << endl;

	log_msg("start zero stations");
	int cnt = agebull::zmq_net::StationWarehouse::restore() + 2;
	while (zero_thread_count < cnt)
	{
		cout << ".";
		thread_sleep(50);
	}
	cout << endl;
	log_msg("����������������");
	return net_state;
}

//�ر����������
void close_net_command(bool wait)
{
	log_msg("zero center closing...");
	if (net_state != NET_STATE_RUNING)
		return;
	agebull::zmq_net::monitor("*", "system_stop", "*************ZeroNet is closed, see you late!*************");
	thread_sleep(10);

	net_state = NET_STATE_CLOSING;
	while (wait && zero_thread_count > 1)
		thread_sleep(10);
	net_state = NET_STATE_CLOSED;
	log_msg("zero center closed");
}
//�������������
void distory_net_command()
{
	if (net_state < NET_STATE_CLOSING)
		close_net_command();
	net_state = NET_STATE_DISTORY;
	zmq_ctx_shutdown(net_context);
	log_msg("zero center distory");
}