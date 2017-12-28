#include "stdafx.h"
#include "ApiRoute.h"
#include <zeromq/zhelpers.h>

#define recv_empty(socket) \
	{\
		char* empty = s_recv(socket, 0);\
		if(empty != nullptr){\
			/*assert(empty[0] == 0);*/\
			free(empty);\
		}\
	}

ApiRoute::ApiRoute(const char* service, const char* routeAddress, const char* workerAddress, const char* heartAddress)
	: _serviceName(service)
	, _routeAddress(routeAddress)
	, _workerAddress(workerAddress)
	, _heartAddress(heartAddress)
	, _nowWorkIndex(0)
	, _routeSocket(nullptr)
	, _heartSocket(nullptr)
	, _workSocket(nullptr)
	, _isRuning(false)
{
}

/*
 *ȡ��һ�����õĹ���������
 *
 LRU�߼�
 -һֱ��_workSocket�л�ȡ��Ϣ�����г���һ��worker����ʱ�Ŵ�_routeSocket��ȡ��Ϣ��
 - ��woker��Ӧʱ���Ὣ��worker���Ϊ��׼���ã���ת��woker�Ļ�Ӧ��client
 - ���client���������󣬾ͽ�������ת������һ��worker
 */
char* ApiRoute::getNextWorker()
{
	if (_workerAddress.size() == 0)
		return nullptr;
	if (_workerAddress.size() == 1)
		return _strdup(_hosts[0].c_str());
	while (_hosts.size() == 0)
	{
		thread_sleep(1);
	}
	auto host = _hosts.back();
	_hosts.pop_back();
	return _strdup(host.c_str());
}
/*
 * ��Զ�̵��ý���ʱ�Ĵ���
 */
void ApiRoute::onCallerPollIn()
{
	// ��ȡ��һ��client�����󣬽������е�worker����
	// client�������Ϣ��ʽ�ǣ�[client��ַ][��֡][��������]
	char* client_addr = s_recv(_routeSocket, 0);
	recv_empty(_routeSocket);
	char* request = s_recv(_routeSocket);

	while (true)
	{
		char* work = getNextWorker();
		if (snedToWorker(work,client_addr, request))
			break;
		leftWorker(work);
	}

	free(client_addr);
	free(request);
}
bool ApiRoute::snedToWorker(char* work ,char* client_addr, char* request)
{
	//·�ɵ�����һ����������
	if (work == nullptr)
	{
		s_sendmore(_routeSocket, client_addr);
		s_sendmore(_routeSocket, "");
		s_send(_routeSocket, "NoWorker");
		return true;
	}
	else
	{
		int state = s_sendmore(_workSocket, work);
		if (state < 0)
			return false;
		state = s_sendmore(_workSocket, "");
		if (state < 0)
			return false;
		state = s_sendmore(_workSocket, client_addr);
		if (state < 0)
			return false;
		state = s_sendmore(_workSocket, "");
		if (state < 0)
			return false;
		state = s_send(_workSocket, request);//��ʵ����
		return state > 0;
	}
}
/*
 * ��������������ʱ�Ĵ���
 */
void ApiRoute::onWorkerPollIn()
{
	// ��worker�ĵ�ַ���
	char* worker_addr = s_recv(_workSocket);
	recv_empty(_workSocket);
	char* client_addr = s_recv(_workSocket);
	recv_empty(_workSocket);
	char* reply = s_recv(_workSocket);
	// �����һ��Ӧ����Ϣ����ת����client
	if (strcmp(client_addr, "READY") == 0)
	{
		joinWorker(worker_addr, reply,true);
	}
	else
	{
		s_sendmore(_routeSocket, client_addr);
		s_sendmore(_routeSocket, "");
		s_send(_routeSocket, reply);//��ʵ����
		_hosts.insert(_hosts.begin(), worker_addr);
	}
	free(worker_addr);
	free(client_addr);
	free(reply);
}

/*
* ��������������ʱ�Ĵ���
*/
void ApiRoute::joinWorker(char* name, char* address, bool ready)
{
	auto old = workers.find(name);
	if (old == workers.end())
	{
		workers.insert(make_pair(string(name), string(address)));
		_hosts.push_back(string(name));
		cout << name << "(" << address << ")�Ѽ���(ͨ��" << (ready ? "����)" : "����)") << endl;
	}
	else
	{
		old->second = address;
		cout << name << "(" << address << ")������(ͨ������)" << endl;
	}
}
/*
* ��������������ʱ�Ĵ���
*/
void ApiRoute::leftWorker(char* name)
{
	auto old = workers.find(name);
	if (old != workers.end())
	{
		workers.erase(name);
		auto iter = _hosts.begin();
		while (iter != _hosts.end())
		{
			if (*iter == name)
			{
				_hosts.erase(iter);
				break;
			}
			iter++;
		}
		cout << name << "���˳�" << endl;
	}
}

/*
* ��������������ʱ�Ĵ���
*/
void ApiRoute::onHeartbeat()
{
	// ��worker�ĵ�ַ���
	char* worker_addr = s_recv(_heartSocket);
	recv_empty(_heartSocket);
	char* client_addr = s_recv(_heartSocket);
	recv_empty(_heartSocket);
	char* reply = s_recv(_heartSocket);
	// �����һ��Ӧ����Ϣ����ת����client
	if (strcmp(client_addr, "PAPA") == 0)
	{
		joinWorker(worker_addr, reply);
	}
	else if (strcmp(client_addr, "MAMA") == 0)
	{
		joinWorker(worker_addr, reply);
	}
	else if (strcmp(client_addr, "LAOWANG") == 0)
	{
		leftWorker(worker_addr);
	}
	size_t size = s_sendmore(_heartSocket, worker_addr);
	if (size <= 0)
		cout << worker_addr << endl;
	size = s_sendmore(_heartSocket, "");
	size = s_send(_heartSocket, "OK");//��ʵ����
	if (size <= 0)
		cout << worker_addr << endl;

	free(worker_addr);
	free(client_addr);
	free(reply);
}
#define init_socket(socket) \
	zmq_setsockopt(socket, ZMQ_LINGER, &iZMQ_IMMEDIATE, sizeof(int));\
	zmq_setsockopt(socket, ZMQ_LINGER, &iLINGER, sizeof(int));\
	zmq_setsockopt(socket, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(int));\
	zmq_setsockopt(socket, ZMQ_SNDTIMEO, &iSndTimeout, sizeof(int));

void ApiRoute::poll()
{
	auto context = get_zmq_context();
	_routeSocket = zmq_socket(context, ZMQ_ROUTER);
	_workSocket = zmq_socket(context, ZMQ_ROUTER);
	_heartSocket = zmq_socket(context, ZMQ_ROUTER);

	int iZMQ_IMMEDIATE = 1;//����Ϣֻ����������ɵ�����
	int iLINGER = 50;//�ر�����ͣ��ʱ��,����
	int iRcvTimeout = 500;
	int iSndTimeout = 500;

	init_socket(_routeSocket);
	init_socket(_workSocket);
	init_socket(_heartSocket);

	zmq_bind(_routeSocket, _routeAddress.c_str());
	zmq_bind(_workSocket, _workerAddress.c_str());
	zmq_bind(_heartSocket, _heartAddress.c_str());

	zmq_pollitem_t items[] = {
		{ _workSocket, 0, ZMQ_POLLIN, 0 },
		{ _routeSocket, 0, ZMQ_POLLIN, 0 },
		{ _heartSocket, 0, ZMQ_POLLIN, 0 }
	};
	log_msg1("Api����(%s)·��������", _serviceName);
	//�Ǽ��߳̿�ʼ
	set_command_thread_start();
	_isRuning = true;
	while (get_net_state() == NET_STATE_RUNING && _isRuning)
	{
		zmq_poll(items, 3, 1000);
		// ����_workSocket��worker�Ķ���
		if (items[0].revents & ZMQ_POLLIN) {
			onWorkerPollIn();
		}

		if (items[1].revents & ZMQ_POLLIN) {
			onCallerPollIn();
		}

		if (items[2].revents & ZMQ_POLLIN) {
			onHeartbeat();
		}
	}
	_isRuning = false;
	zmq_unbind(_routeSocket, _routeAddress.c_str());
	zmq_close(_routeSocket);
	zmq_unbind(_workSocket, _workerAddress.c_str());
	zmq_close(_workSocket);
	zmq_unbind(_heartSocket, _heartAddress.c_str());
	zmq_close(_heartSocket);
	//�Ǽ��̹߳ر�
	set_command_thread_end();
	log_msg1("Api����(%s)·���ѹر�", _serviceName);
}

ApiRoute::~ApiRoute()
{
	if (!_isRuning)
		return;
	_isRuning = false;
	thread_sleep(1000);
}
