// WindowsService.cpp : WinMain ��ʵ��

#include "stdafx.h"
#include "rpc/CRpcService.h"
#include "api/ApiRoute.h"
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
int main(void)
{
	char buffer[MAX_PATH + 1];
	char *p = _getcwd(buffer, MAX_PATH);
	cout << p << endl;
	CRpcService::Initialize();
	CRpcService::Start();
	ApiRoute route("test", "tcp://*:10001", "tcp://*:10002", "tcp://*:10000");
	startRoute(route);
	char c;
	std::cout << endl << "������ɣ������������";
	std::cin >> c;
	CRpcService::Stop();
	thread_sleep(1000);
	std::cout << endl << "�ѹر�";
	return 0;
}
#endif