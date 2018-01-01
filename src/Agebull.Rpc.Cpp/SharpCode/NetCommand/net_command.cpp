#include "stdinc.h"
#include "net_command.h"
#include "debug/TraceStack.h"
#include "ApiStation.h"
#include "BroadcastingStation.h"
using namespace std;

ZMQ_HANDLE net_context;
volatile NET_STATE net_state = NET_STATE_NONE;
//��ǰ�����˶��������߳�
volatile int command_thread_count;
//ZMQ�����Ķ���
ZMQ_HANDLE get_zmq_context()
{
	return net_context;
}

//�Ǽ��߳̿�ʼ
void set_command_thread_start()
{
	command_thread_count++;
	log_msg1("���紦���߳�����%d->����", command_thread_count);
}
//�Ǽ��̹߳ر�
void set_command_thread_end()
{
	command_thread_count--;
	log_msg1("���紦���߳�����%d->�ر�", command_thread_count);
}
//����״̬
NET_STATE get_net_state()
{
	return net_state;
}
//#ifdef COMMANDPROXY
//CommandProxy* proxy = new CommandProxy();
//#endif
//��ʼ�����������
int init_net_command()
{
	log_msg("��ʼ�����������...");
	net_state = NET_STATE_NONE;
	net_context = zmq_ctx_new();

	assert(net_context != nullptr);

	zmq_ctx_set(net_context, ZMQ_MAX_SOCKETS, 8192);
	zmq_ctx_set(net_context, ZMQ_IO_THREADS, 16);
	zmq_ctx_set(net_context, ZMQ_MAX_MSGSZ, 8192);


	//boost::thread smp(boost::bind(&server_message_pump));

	thread_sleep(50);
	log_msg("��������������ʼ��");
	return net_state;
}
//�������������
int start_net_command()
{
	log_msg("�����������������...");
	net_state = NET_STATE_RUNING;

	int cnt = agebull::zmq_net::StationWarehouse::restore();

	while (command_thread_count < cnt)
		thread_sleep(10);
	log_msg("����������������");
	return net_state;
}

//�ر����������
void close_net_command()
{
	log_msg("���ڹر����������...");
	if (net_state != NET_STATE_RUNING)
		return;
	net_state = NET_STATE_CLOSING;
	while (command_thread_count > 0)
		thread_sleep(10);
	net_state = NET_STATE_CLOSED;
	log_msg("����������ѹر�");
}
//�������������
void distory_net_command()
{
	close_net_command();
	net_state = NET_STATE_DISTORY;
	log_msg("���������������");
}