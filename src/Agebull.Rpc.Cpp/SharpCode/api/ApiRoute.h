#pragma once
#include <stdinc.h>

class ApiRoute
{
	/*
	* @brief API��������
	*/
	string _serviceName;

	/*
	* @brief ·�ɵ�ַ
	*/
	string _routeAddress;

	/*
	* @brief ������ַ
	*/
	string _workerAddress;

	/*
	* @brief ������ַ
	*/
	string _heartAddress;
	/*
	* @brief ���þ��
	*/
	void* _routeSocket;

	/*
	* @brief �������
	*/
	void* _workSocket;
	/*
	* @brief �������
	*/
	void* _heartSocket;

	/*
	* @brief Ӧ�ü���ִ��
	*/
	bool _isRuning;

	/*
	* @brief API��������
	*/
	map<string, string> workers;
	/*
	* @brief API��������
	*/
	vector<string> _hosts;

	/*
	* @brief ��ǰ�������±�
	*/
	int _nowWorkIndex;
public:
	/*
	* @brief ����
	*/
	ApiRoute(const char* service, const char* routeAddress, const char* workerAddress, const char* heartAddress);
	/*
	* @brief ����
	*/
	~ApiRoute();
	/*
	* @brief ִ��
	*/
	static DWORD ApiRoute::start(ApiRoute& route)
	{
		route.poll();
		return 0;
	};
	/*
	* @brief ����
	*/
	static DWORD ApiRoute::end(ApiRoute& route)
	{
		route._isRuning = false;
		thread_sleep(1000);
		return 0;
	};
private:
	/*
	* @brief ִ��
	*/
	void poll();
	/*
	* @brief �������ϵ���Ӧ
	*/
	void onWorkerPollIn();
	/*
	* @brief ���ü��ϵ���Ӧ
	*/
	void onCallerPollIn();
	bool snedToWorker(char* work, char* client_addr, char* request);
	/*
	* ��������Ӧ
	*/
	void onHeartbeat();

	/*
	* @brief ȡ��һ����������
	*/
	char* getNextWorker();
	void leftWorker(char* name);
	void joinWorker(char* name, char* address,bool ready=false);
};
/*
 * ����һ��·���߳�
 */
inline void startRoute(ApiRoute& route)
{
	boost::thread thrds_s1(boost::bind(ApiRoute::start, route));
}
