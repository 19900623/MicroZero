// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// StreamTest.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
#include "service.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <execinfo.h>
#include <string>
#include "net/station_warehouse.h"
#include "net/station_dispatcher.h"


namespace agebull
{
	namespace zmq_net
	{
		const int MAX_STACK_FRAMES = 128;


		/**
		* \brief sig��Ӧ���ı�
		*/
		const char* sig_text(int sig)
		{
			switch (sig)
			{
			case SIGABRT: return "�ɵ���abort�������������̷������˳�";
			case SIGKILL: return "�޷�����ͺ��ԡ���ֹĳ������";
			case SIGTERM: return "������ֹ���̣�kill����ȱʡ����";
			case SIGINT: return "��Interrupt Key������ͨ����CTRL + C����DELETE�����͸�����ForeGround Group�Ľ���";

			case SIGALRM: return "��alarm�������õ�timer��ʱ��setitimer�������õ�interval timer��ʱ";
			case SIGBUS: return "ĳ���ض���Ӳ���쳣��ͨ�����ڴ��������";
				//case SIGCANCEL: return "��Solaris Thread Library�ڲ�ʹ�ã�ͨ������ʹ��";
			case SIGCHLD: return "����Terminate��Stop��ʱ��SIGCHLD�ᷢ�͸����ĸ����̡�ȱʡ����¸�Signal�ᱻ����";
			case SIGCONT: return "����stop�Ľ��ָ̻����е�ʱ���Զ�����";
				//case SIGEMT: return "��ʵ����ص�Ӳ���쳣
			case SIGFPE: return "��ѧ��ص��쳣���类0��������������ȵ�";
				//case SIGFREEZE: return "Solarisר�ã�Hiberate����Suspendedʱ����
			case SIGHUP: return "���͸�����Terminal��Controlling Process����terminal��disconnectʱ����";
			case SIGILL: return "�Ƿ�ָ���쳣";
				//case SIGINFO: return "BSD signal����Status Key������ͨ����CTRL + T�����͸�����Foreground Group�Ľ���
			case SIGIO: return "�첽IO�¼�";
				//case SIGIOT: return "ʵ����ص�Ӳ���쳣��һ���ӦSIGABRT";
				//case SIGLWP: return "��Solaris Thread Libray�ڲ�ʹ��
			case SIGPIPE: return "��reader��ֹ֮��дPipe��ʱ����";
				//case SIGPOLL: return "��ĳ���¼����͸�Pollable Device��ʱ����";
			case SIGPROF: return "Setitimerָ����Profiling Interval Timer������";
			case SIGPWR: return "��ϵͳ��ء���UPS��ء�";
			case SIGQUIT: return "����Quit Key��ʱ��CTRL + \\�����͸�����Foreground Group�Ľ���";
			case SIGSEGV: return "�Ƿ��ڴ����";
			case SIGSTKFLT: return "Linuxר�ã���ѧЭ��������ջ�쳣";
			case SIGSTOP: return "��ֹ���̡��޷�����ͺ��ԡ�";
			case SIGSYS: return "�Ƿ�ϵͳ����";
				//case SIGTHAW: return "Solarisר�ã���Suspend�ָ�ʱ����";
			case SIGTRAP: return "ʵ����ص�Ӳ���쳣��һ���ǵ����쳣";
			case SIGTSTP: return "Suspend Key��һ����Ctrl + Z�����͸�����Foreground Group�Ľ���";
			case SIGTTIN: return "��Background Group�Ľ��̳��Զ�ȡTerminal��ʱ����";
			case SIGTTOU: return "��Background Group�Ľ��̳���дTerminal��ʱ����";
			case SIGURG: return "��out - of - band data���յ�ʱ����ܷ���";
			case SIGUSR1: return "�û��Զ���signal 1";
			case SIGUSR2: return "�û��Զ���signal 2";
			case SIGVTALRM: return "setitimer�������õ�Virtual Interval Timer��ʱ��ʱ��";
				//case SIGWAITING: return "Solaris Thread Library�ڲ�ʵ��ר��";
			case SIGWINCH: return "��Terminal�Ĵ��ڴ�С�ı��ʱ�򣬷��͸�Foreground Group�����н���";
			case SIGXCPU: return "��CPUʱ�����Ƴ�ʱ��ʱ��";
			case SIGXFSZ: return "���̳����ļ���С����";
				//case SIGXRES: return "Solarisר�ã����̳�����Դ���Ƶ�ʱ����
			default:
				return "δ֪��ֹԭ��";

			}

		}
		/**
		* \brief ϵͳ�źŴ���
		*/
		void on_sig(int sig) {
			sig_crash(sig);
			switch (sig)
			{
			case SIGABRT://�ɵ���abort�������������̷������˳�
			case SIGKILL://�޷�����ͺ��ԡ���ֹĳ������
			case SIGTERM://������ֹ���̣�kill����ȱʡ����
			case SIGINT://��Interrupt Key������ͨ����CTRL + C����DELETE�����͸�����ForeGround Group�Ľ���
				boost::thread(boost::bind(rpc_service::stop));
				break;
				/*
				case SIGALRM://��alarm�������õ�timer��ʱ��setitimer�������õ�interval timer��ʱ
				case SIGBUS://ĳ���ض���Ӳ���쳣��ͨ�����ڴ��������
				case SIGCANCEL://��Solaris Thread Library�ڲ�ʹ�ã�ͨ������ʹ��
				case SIGCHLD://����Terminate��Stop��ʱ��SIGCHLD�ᷢ�͸����ĸ����̡�ȱʡ����¸�Signal�ᱻ����
				case SIGCONT://����stop�Ľ��ָ̻����е�ʱ���Զ�����
				case SIGEMT://��ʵ����ص�Ӳ���쳣
				case SIGFPE://��ѧ��ص��쳣���类0��������������ȵ�
				case SIGFREEZE://Solarisר�ã�Hiberate����Suspendedʱ����
				case SIGHUP://���͸�����Terminal��Controlling Process����terminal��disconnectʱ����
				case SIGILL://�Ƿ�ָ���쳣
				case SIGINFO://BSD signal����Status Key������ͨ����CTRL + T�����͸�����Foreground Group�Ľ���
				case SIGIO://�첽IO�¼�
				case SIGIOT://ʵ����ص�Ӳ���쳣��һ���ӦSIGABRT
				case SIGLWP://��Solaris Thread Libray�ڲ�ʹ��
				case SIGPIPE://��reader��ֹ֮��дPipe��ʱ����
				case SIGPOLL://��ĳ���¼����͸�Pollable Device��ʱ����
				case SIGPROF://Setitimerָ����Profiling Interval Timer������
				case SIGPWR://��ϵͳ��ء���UPS��ء�
				case SIGQUIT://����Quit Key��ʱ��CTRL + \�����͸�����Foreground Group�Ľ���
				case SIGSEGV://�Ƿ��ڴ����
				case SIGSTKFLT://Linuxר�ã���ѧЭ��������ջ�쳣
				case SIGSTOP://��ֹ���̡��޷�����ͺ��ԡ�
				case SIGSYS://�Ƿ�ϵͳ����
				case SIGTHAW://Solarisר�ã���Suspend�ָ�ʱ����
				case SIGTRAP://ʵ����ص�Ӳ���쳣��һ���ǵ����쳣
				case SIGTSTP://Suspend Key��һ����Ctrl + Z�����͸�����Foreground Group�Ľ���
				case SIGTTIN://��Background Group�Ľ��̳��Զ�ȡTerminal��ʱ����
				case SIGTTOU://��Background Group�Ľ��̳���дTerminal��ʱ����
				case SIGURG://��out - of - band data���յ�ʱ����ܷ���
				case SIGUSR1://�û��Զ���signal 1
				case SIGUSR2://�û��Զ���signal 2
				case SIGVTALRM://setitimer�������õ�Virtual Interval Timer��ʱ��ʱ��
				case SIGWAITING://Solaris Thread Library�ڲ�ʵ��ר��
				case SIGWINCH://��Terminal�Ĵ��ڴ�С�ı��ʱ�򣬷��͸�Foreground Group�����н���
				case SIGXCPU://��CPUʱ�����Ƴ�ʱ��ʱ��
				case SIGXFSZ://���̳����ļ���С����
				case SIGXRES://Solarisר�ã����̳�����Դ���Ƶ�ʱ����
				default:
				return;*/

			}

			rpc_service::wait_semaphore.post();
		}

		/**
		* \brief ��¼��ջ��Ϣ
		*/
		void sig_crash(int sig)
		{
			try
			{
				time_t t = time(nullptr);
				tm* now = localtime(&t);
				log_error8(
					"#########################################################\n[%04d-%02d-%02d %02d:%02d:%02d][crash signal number:%d]\n[%s]",
					now->tm_year + 1900,
					now->tm_mon + 1,
					now->tm_mday,
					now->tm_hour,
					now->tm_min,
					now->tm_sec,
					sig, sig_text(sig));
#ifdef __linux
				signal(sig, SIG_DFL);
				void* array[MAX_STACK_FRAMES];
				char** strings = nullptr;
				var size = backtrace(array, MAX_STACK_FRAMES);
				strings = (char**)backtrace_symbols(array, size);
				for (int i = 0; i < size; ++i)
				{
					log_error2("%d %s", i, strings[i]);
				}
				free(strings);
#endif // __linux
			}
			catch (...)
			{
				log_error("exception");
			}
		}

		/**
		* \brief ȡ����IP
		*/
		void get_local_ips(acl::string& host, vector<acl::string>& ips)
		{
			char hname[128];
			memset(hname, 0, sizeof(hname));
			gethostname(hname, sizeof(hname));
			host = hname;
			struct addrinfo hint {};
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
					inet_ntop(AF_INET, &(reinterpret_cast<struct sockaddr_in *>(now->ai_addr)->sin_addr), ipstr, 16);
					ips.emplace_back(ipstr);
					now = now->ai_next;
				} while (now != nullptr);
				freeaddrinfo(info);
			}
		}

		/**
		* \brief ��ȡָ����������Ӧ�Ŀ�ִ�У�EXE���ļ�ȫ·��
		* \param sFilePath - ���̾��hProcess����Ӧ�Ŀ�ִ���ļ�·��
		* /
		void get_process_file_path(string& sFilePath)
		{
		#if WIN32

		char tsFileDosPath[MAX_PATH + 1];
		ZeroMemory(tsFileDosPath, sizeof(char)*(MAX_PATH + 1));

		HANDLE hProcess = GetCurrentProcess();
		DWORD re = GetProcessImageFileNameA(hProcess, tsFileDosPath, MAX_PATH + 1);
		CloseHandle(hProcess);
		if (0 == re)
		{
		return;
		}

		// ��ȡLogic Drive String����
		UINT uiLen = GetLogicalDriveStrings(0, nullptr);
		if (0 == uiLen)
		{
		return;
		}

		char* pLogicDriveString = new char[uiLen + 1];
		ZeroMemory(pLogicDriveString, uiLen + 1);
		uiLen = GetLogicalDriveStringsA(uiLen, pLogicDriveString);
		if (0 == uiLen)
		{
		delete[]pLogicDriveString;
		return;
		}

		char szDrive[3] = " :";
		char* pDosDriveName = new char[MAX_PATH];
		char* pLogicIndex = pLogicDriveString;

		do
		{
		szDrive[0] = *pLogicIndex;
		uiLen = QueryDosDeviceA(szDrive, pDosDriveName, MAX_PATH);
		if (0 == uiLen)
		{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
		{
		break;
		}

		delete[]pDosDriveName;
		pDosDriveName = new char[uiLen + 1];
		uiLen = QueryDosDeviceA(szDrive, pDosDriveName, uiLen + 1);
		if (0 == uiLen)
		{
		break;
		}
		}

		uiLen = strlen(pDosDriveName);
		if (0 == _strnicmp(tsFileDosPath, pDosDriveName, uiLen))
		{
		sFilePath.append(szDrive);
		sFilePath.append(tsFileDosPath + uiLen);
		break;
		}

		while (*pLogicIndex++);
		} while (*pLogicIndex);

		delete[]pLogicDriveString;
		delete[]pDosDriveName;
		#else
		char buf[512];
		getcwd(buf, 512);
		sFilePath = buf;
		#endif
		}*/

	}
}