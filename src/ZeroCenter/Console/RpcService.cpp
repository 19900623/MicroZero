// WindowsService.cpp : WinMain ��ʵ��

#include "stdafx.h"
#include "rpc/CRpcService.h"
#include "NetCommand/ZeroStation.h"
#include "NetCommand/NetDispatcher.h"
#include "NetCommand/BroadcastingStation.h"
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>  
#endif

#ifdef WINDOWS_SERVICE

#include "resource.h"
#include "WindowsService_i.h"


using namespace ATL;

#include <stdio.h>

class CWindowsServiceModule : public ATL::CAtlServiceModuleT< CWindowsServiceModule, IDS_SERVICENAME >
{
public:
	DECLARE_LIBID(LIBID_WindowsServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WINDOWSSERVICE, "{159498A2-1A83-4EA7-8003-9C03F90641AA}")
	HRESULT InitializeSecurity() throw();

	/**
	* \brief
	* \param bService
	* \return
	*/
	HRESULT RegisterAppId(bool bService)  throw ();

	/**
	* \brief
	* \param nShowCmd
	* \return
	*/
	HRESULT PreMessageLoop(int nShowCmd) throw();

	/**
	* \brief
	* \return
	*/
	HRESULT PostMessageLoop() throw();

	/**
	* \brief
	*/
	void OnStop() throw();

	/**
	* \brief
	*/
	void OnPause() throw();

	/**
	* \brief
	*/
	void OnContinue() throw();
};

HRESULT CWindowsServiceModule::InitializeSecurity() throw()
{
	// TODO : ���� CoInitializeSecurity ��Ϊ�����ṩ�ʵ��İ�ȫ����
	// ���� - PKT ����������֤��
	// RPC_C_IMP_LEVEL_IDENTIFY ��ģ�⼶��
	// �Լ��ʵ��ķ� NULL ��ȫ��������

	return S_OK;
}

HRESULT CWindowsServiceModule::RegisterAppId(bool bService) throw()
{
	log_acl_msg("RegisterAppId");
	__super::RegisterAppId(bService);

	HRESULT hr = S_OK;
	if (bService)
	{
		if (IsInstalled())
		{
			SC_HANDLE hSCM = ::OpenSCManagerW(nullptr, nullptr, SERVICE_CHANGE_CONFIG);
			SC_HANDLE hService = nullptr;
			if (hSCM == nullptr)
			{
				hr = AtlHresultFromLastError();
			}
			else
			{
				hService = ::OpenService(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
				if (hService != nullptr)
				{
					::ChangeServiceConfig(hService, SERVICE_NO_CHANGE,
						SERVICE_AUTO_START,// �޸ķ���Ϊ�Զ�����
						NULL, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
						m_szServiceName); // ͨ���޸���ԴIDS_SERVICENAME �޸ķ������ʾ����

					SERVICE_DESCRIPTION Description;
					TCHAR szDescription[1024];
					ZeroMemory(szDescription, 1024);
					ZeroMemory(&Description, sizeof(SERVICE_DESCRIPTION));
					lstrcpy(szDescription, _T("���Է���������Ϣ"));
					Description.lpDescription = szDescription;
					::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
					::CloseServiceHandle(hService);
					log_acl_msg("Registe");
				}
				else
				{
					log_acl_msg("Fined");
					hr = AtlHresultFromLastError();
				}
				::CloseServiceHandle(hSCM);
			}
		}
		else
		{
			log_acl_msg("no Installed");
		}
	}
	log_acl_msg("RegisterAppId***");
	return hr;
}

HRESULT CWindowsServiceModule::PreMessageLoop(int nShowCmd) throw()
{
	log_acl_msg("PreMessageLoop");
	// ����ͣ������ť����ʹ��
	m_status.dwControlsAccepted = m_status.dwControlsAccepted | SERVICE_ACCEPT_PAUSE_CONTINUE;

	HRESULT hr = __super::PreMessageLoop(nShowCmd);
	// ΢��Bug
	if (hr == S_FALSE)
		hr = S_OK;

	if (SUCCEEDED(hr))
	{
		CRpcService::Start();
		// ���״̬һ��Ҫ�޸ģ���������1053����
		// ��������һ��˺ܶ�ʱ��Ÿ㶨
		SetServiceStatus(SERVICE_RUNNING);
	}

	return hr;
}

HRESULT CWindowsServiceModule::PostMessageLoop() throw()
{
	HRESULT hr = __super::PostMessageLoop();

	if (FAILED(hr))
	{
		log_acl_msg("PostMessageLoop FAILED");
		return hr;
	}

	log_acl_msg("PostMessageLoop succed");
	SetServiceStatus(SERVICE_RUNNING);
	return hr;
}

void CWindowsServiceModule::OnStop() throw()
{
	CRpcService::Stop();
	__super::OnStop();
	SetServiceStatus(SERVICE_STOPPED);
}

void CWindowsServiceModule::OnPause() throw()
{
	log_acl_msg("OnPause");
	__super::OnPause();
	SetServiceStatus(SERVICE_PAUSED);
}

void CWindowsServiceModule::OnContinue() throw()
{
	log_acl_msg("OnContinue");
	//��ʼ�������
	__super::OnContinue();
	SetServiceStatus(SERVICE_RUNNING);
}


CWindowsServiceModule _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
	LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	CRpcService::Initialize();
	return _AtlModule.WinMain(nShowCmd);
}
#else
#include<winsock2.h>  
#pragma comment(lib,"ws2_32.lib") // ��̬��  

void getIPs()
{
	char hname[128];
	gethostname(hname, sizeof(hname));
	cout << "Host:" << hname << "  IP:";
	struct addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	addrinfo* info = nullptr;
	char ipstr[16];
	if (getaddrinfo(hname, nullptr, &hint, &info) == 0 && info != nullptr)
	{
		addrinfo* now = info;
		do
		{
			inet_ntop(AF_INET, &(((struct sockaddr_in *)(now->ai_addr))->sin_addr), ipstr, 16);
			cout << ipstr << " ";
			now = now->ai_next;
		} while (now != nullptr);
		freeaddrinfo(info);
	}
	cout << endl;
}

int main(int argc, char *argv[])
{
	getIPs();
	//WORD v = MAKEWORD(1, 1);
	//WSADATA wsaData;
	//WSAStartup(v, &wsaData); // �����׽��ֿ�    
	//int i = 0;
	//while (i < argc)
	//	std::cout << argv[i++] << " ";
	//std::cout << endl;
	char buffer[MAX_PATH + 1];
	char *p = _getcwd(buffer, MAX_PATH);
	cout << p << endl;
	
	CRpcService::Initialize();
	CRpcService::Start();
	while (get_net_state() == NET_STATE_RUNING)
	{
		std::cout << endl << "�������������(exit):";
		string line;
		getline(cin, line);
		if (line.length() == 0)
			break;
		if(line == "clear")
		{
			agebull::zmq_net::StationWarehouse::clear();
			break;
		}
		std::vector<string> cmdline;
		// boost::is_any_of�����൱�ڷָ������  
		boost::split(cmdline, line, boost::is_any_of(" \n\r\t"));
		if (cmdline.empty())
			break;
		std::vector<agebull::zmq_net::sharp_char> arguments;
		for (size_t idx = 1; idx < cmdline.size(); idx++)
			arguments.push_back(cmdline[idx]);
		string result = agebull::zmq_net::NetDispatcher::exec_command(cmdline[0].c_str(), arguments);
		std::cout << result << endl;
	}
	CRpcService::Stop();
	std::cout << endl << "�ѹر�";
	thread_sleep(200);
	return 0;
}
#endif