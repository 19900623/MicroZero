// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// WindowsService.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

#ifdef CONSOLE_TEST

//
// ʹ��LRU�㷨��װ��
// client��worker���ڲ�ͬ���߳���
//
#include <zeromq/zhelpers.h>

#define NBR_CLIENTS 10
#define NBR_WORKERS 3

// ���Ӳ�����ʹ��һ���ɴ洢�κ����͵�����ʵ��
#define DEQUEUE(q) memmove (&(q)[0], &(q)[1], sizeof (q) - sizeof (q [0]))

// ʹ��REQ�׽���ʵ�ֻ���������-Ӧ��ģʽ
// ����s_send()��s_recv()���ܴ���0MQ�Ķ������׽��ֱ�ʶ��
// �������������һ���ɴ�ӡ���ַ�����ʶ��
//
static DWORD client_task(LPVOID arg)
{
	void *context = zmq_init(1);
	void *client = zmq_socket(context, ZMQ_REQ);

	int id = (int)arg + 1;
	s_set_id(client, id); // ���ÿɴ�ӡ�ı�ʶ
	int re = zmq_connect(client, "tcp://127.0.0.1:6666");
	char identity[10];
	sprintf(identity, "%04X", (int)id);
	cout << "client connect" << re << "--" << identity << endl;

	// �������󲢻�ȡӦ����Ϣ
	s_send(client, "HELLO");
	cout << endl << "client send" << endl;
	char *reply = s_recv(client);
	cout << "-------------------Client:" << identity <<",Rep:" << reply << endl;
	free(reply);
	zmq_close(client);
	zmq_term(context);
	return 0;
}

// workerʹ��REQ�׽���ʵ��LRU�㷨
//
static DWORD worker_task(LPVOID arg)
{
	void *context = zmq_init(1);
	void *worker = zmq_socket(context, ZMQ_REQ);

	int id = (int)arg + 1;
	s_set_id(worker, id); // ���ÿɴ�ӡ�ı�ʶ
	int re = zmq_connect(worker, "tcp://127.0.0.1:7777");

	char identity[10];
	sprintf(identity, "%04X", (int)id);
	cout << "worker connect" << re << "--" << identity << endl;
	// ���ߴ���worker�Ѿ�׼����
	s_send(worker, "READY");

	cout << "worker ready" << re << endl;
	while (1) {
		// ����Ϣ�п�֮֡ǰ���������ݣ��ŷ⣩����������
		// �����п�֮֡ǰֻ��һ֡���������и��ࡣ
		char *address = s_recv(worker);      //1����ȡ�ŷ��ַ
		char *empty = s_recv(worker);        //2����ȡ ��֡
		assert(*empty == 0);
		free(empty);

		// ��ȡ���󣬲����ͻ�Ӧ
		char *request = s_recv(worker);      //3����ȡ ��Ϣ
		cout << "``````````````````Worker:"<< identity << ",Client:" << address << ",Request:" << request << endl;
		free(request);
		if (strcmp(address, "Bye") == 0)
		{
			free(address);
			break;
		}
		//��װ��Ϣ
		s_sendmore(worker, address);      //��ַ�ŷ�
		s_sendmore(worker, "");              //��֡
		s_send(worker, "OK");                //��ʵ��Ϣ
		free(address);
	}
	zmq_close(worker);
	zmq_term(context);
	return 0;
}
int client_nbr;
int worker_nbr;


DWORD route(LPVOID arg)
{
	auto name = static_cast<char*>(arg);

	// ׼��0MQ�����ĺ��׽���
	void *context = zmq_init(1);
	void *frontend = zmq_socket(context, ZMQ_ROUTER);
	void *backend = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(frontend, "tcp://127.0.0.1:6666");
	zmq_bind(backend, "tcp://127.0.0.1:7777");

	// LRU�߼�
	// - һֱ��backend�л�ȡ��Ϣ�����г���һ��worker����ʱ�Ŵ�frontend��ȡ��Ϣ��
	// - ��woker��Ӧʱ���Ὣ��worker���Ϊ��׼���ã���ת��woker�Ļ�Ӧ��client
	// - ���client���������󣬾ͽ�������ת������һ��worker

	zmq_pollitem_t items[] = {
		{ backend, 0, ZMQ_POLLIN, 0 },
		{ frontend, 0, ZMQ_POLLIN, 0 }
	};
	// ��ſ���worker�Ķ���
	int available_workers = 0;
	char *worker_queue[10];
	cout << endl << "route ready" << endl;
	while (1) {
		zmq_poll(items, available_workers ? 2 : 1, -1);

		cout << "route poll";
		// ����backend��worker�Ķ���
		if (items[0].revents & ZMQ_POLLIN) {
			// ��worker�ĵ�ַ���
			char *worker_addr = s_recv(backend);
			assert(available_workers < NBR_WORKERS);

			// ������֡
			char *empty = s_recv(backend);
			assert(empty[0] == 0);
			free(empty);

			// ����֡�ǡ�READY������һ��client�ĵ�ַ
			char *client_addr = s_recv(backend);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(client_addr, "READY") == 0)
			{
				worker_queue[available_workers++] = worker_addr;
			}
			else if (strcmp(client_addr, "BYE") == 0)
			{
				worker_queue[available_workers++] = worker_addr;
			}
			else
			{
				empty = s_recv(backend);
				assert(empty[0] == 0);
				free(empty);
				char *reply = s_recv(backend);
				s_sendmore(frontend, client_addr);
				s_sendmore(frontend, "");
				s_send(frontend, reply);      
				cout << "******Result:" << client_addr<<",Res:"<< reply << endl;
				free(reply);
				if (--client_nbr == 0)
					break; // ����N����Ϣ���˳�
			}
			cout << endl;
			free(client_addr);
		}

		if (items[1].revents & ZMQ_POLLIN) {
			// ��ȡ��һ��client�����󣬽������е�worker����
			// client�������Ϣ��ʽ�ǣ�[client��ַ][��֡][��������]
			char *client_addr = s_recv(frontend);
			cout << "*2*" << client_addr << endl;
			char *empty = s_recv(frontend);
			assert(empty[0] == 0);
			free(empty);
			char *request = s_recv(frontend);

			//�������һ����Ϣ
			s_sendmore(backend, worker_queue[0]);
			s_sendmore(backend, "");
			s_sendmore(backend, client_addr);
			s_sendmore(backend, "");
			s_send(backend, request);      
			cout << "******Request:" << client_addr << ",Arg:" << request << endl;

			free(client_addr);
			free(request);

			// ����worker�ĵ�ַ����
			free(worker_queue[0]);
			DEQUEUE(worker_queue);
			available_workers--;
		}
	}
	zmq_close(frontend);
	zmq_close(backend);
	zmq_term(context);
	cout << "route end" << endl;
	return 0;
}
int main(void)
{
	start_thread(route, LPVOID(NULL));
	thread_sleep(100);
	for (client_nbr = 0; client_nbr < NBR_CLIENTS; client_nbr++) {
		start_thread(client_task, LPVOID(client_nbr));
	}
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++) {
		start_thread(worker_task, LPVOID(worker_nbr));
	}
	getchar();
	return 0;
}
#endif
