#include "stdinc.h"
#include "net_command.h"
#include "debug/TraceStack.h"
#ifdef CLR
#pragma unmanaged
#endif
//#ifdef EsTrade
//#include "EsTrade\EsTradeCommand.h"
//typedef Agebull::Futures::EsProxy::EsTradeCommand CommandProxy;
//#endif
//#ifdef GBS_TRADE
//#include <InnerTrade\GbsTradeCommand.h>
//typedef Agebull::Rpc::GbsTradeCommand CommandProxy;
//#endif
//#ifdef GBS_TRADE
//#include <InnerTrade\GbsTradeCommand.h>
//typedef Agebull::Rpc::GbsTradeCommand CommandProxy;
//#endif
//#ifdef EsQuote
//#include "Quote\Es30QuoteNotify.h"
//#include "Quote\EsQuoteNotify.h"
//typedef EsQuoteNotify CommandProxy;
//#endif

using namespace std;

ZMQ_HANDLE net_context;
NET_STATE net_state = NET_STATE_NONE;
//��ǰ�����˶��������߳�
int command_thread_count;
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


	boost::thread smp(boost::bind(&server_message_pump));

	thread_sleep(50);
	log_msg("��������������ʼ��");
	return net_state;
}
//�������������
int start_net_command()
{
	log_msg("�����������������...");
	net_state = NET_STATE_RUNING;
	int cnt = 1;
#ifdef CLIENT_COMMAND
	//start_thread(zmq_monitor, "inproc://request_cmd.rep");
	//thread_sleep(1);
	//start_thread(zmq_monitor, "inproc://notify_sub.rep");
	//thread_sleep(1);
#define NET_CNT
	cnt = 2;
	start_thread(client_sub, LPVOID(pub_addr));
	start_thread(client_cmd, LPVOID(cmd_addr));
	if (quote_addr != nullptr && strlen(quote_addr) > 0)
	{
		cnt = 3;
		start_thread(quote_sub, LPVOID(quote_addr));
	}
#endif
#ifdef	EsQuote
	boost::thread thrds_s1(boost::bind(&server_notify));
#endif
#ifdef COMMANDPROXY
	//start_thread(zmq_monitor, "inproc://server_pub.rep");
	//thread_sleep(1);
	//start_thread(zmq_monitor, "inproc://server_request.rep");
	////thread_sleep(1);
	boost::thread thrds_s1(boost::bind(&server_request));
	boost::thread thrds_s2(boost::bind(&server_answer));
	boost::thread thrds_s3(boost::bind(&server_notify));
#ifdef GBS_TRADE
	cnt = 5;
	boost::thread thrds_b1(boost::bind(&proxy_sub));
	boost::thread thrds_b2(boost::bind(&proxy_cmd));
#else
	cnt = 3;
#endif
#endif
#ifdef INNER_SETTLEMENT
	cnt = 2 + quote_sub();
	boost::thread thrds_b1(boost::bind(&proxy_sub));
	boost::thread thrds_b2(boost::bind(&proxy_cmd));
#endif
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
#ifdef INNER_SERVER
	//����ͻ���
	distory_client();
#endif
#ifdef CLIENT_COMMAND
	//����ͻ���
	distory_client();
#endif
	net_state = NET_STATE_DISTORY;
	log_msg("���������������");
}