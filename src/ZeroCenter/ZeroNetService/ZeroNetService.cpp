// ZeroNetService.cpp : WinMain ��ʵ��


#include "stdafx.h"
#include "resource.h"
#include "ZeroNetService_i.h"
#include "rpc/crpcservice.h"
#include "NetCommand/ZeroStation.h"
#include "NetCommand/NetDispatcher.h"
#include "NetCommand/BroadcastingStation.h"
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_FREE_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

using namespace agebull;
using namespace ATL;

class CZeroNetServiceModule : public ATL::CAtlServiceModuleT< CZeroNetServiceModule, IDS_SERVICENAME >
{
public:
	DECLARE_LIBID(LIBID_ZeroNetServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ZERONETSERVICE, "{C654FDFF-ECA0-46E2-AF11-BBD417BD2417}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : ���� CoInitializeSecurity ��Ϊ�����ṩ�ʵ��İ�ȫ����
		// ���� - PKT ����������֤��
		// RPC_C_IMP_LEVEL_IDENTIFY ��ģ�⼶��
		// �Լ��ʵ��ķ� NULL ��ȫ��������

		return S_OK;
	}

	HRESULT RegisterAppId(_In_ bool bService=false) throw()
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
						lstrcpy(szDescription, _T("ZeroNet��Ϣ���ķ���"));
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

	HRESULT PreMessageLoop(int nShowCmd) throw()
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
			rpc_service::start();
			// ���״̬һ��Ҫ�޸ģ���������1053����
			// ��������һ��˺ܶ�ʱ��Ÿ㶨
			SetServiceStatus(SERVICE_RUNNING);
		}

		return hr;
	}

	HRESULT PostMessageLoop() throw()
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

	void OnStop() throw()
	{
		rpc_service::stop();
		__super::OnStop();
		SetServiceStatus(SERVICE_STOPPED);
	}

	void OnPause() throw()
	{
		log_acl_msg("OnPause");
		__super::OnPause();
		SetServiceStatus(SERVICE_PAUSED);
	}

	void OnContinue() throw()
	{
		log_acl_msg("OnContinue");
		//��ʼ�������
		__super::OnContinue();
		SetServiceStatus(SERVICE_RUNNING);
	}
};

CZeroNetServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
	LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

