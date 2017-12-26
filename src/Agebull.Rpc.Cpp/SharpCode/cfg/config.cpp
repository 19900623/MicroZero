#include "config.h"
#include "Psapi.h"

std::map<std::string, std::string> config::m_cfg;


void config::init()
{
	if (m_cfg.empty())
	{
		std::string path = "";
		GetProcessFilePath(path);
		path.append(".json");
		log_acl_msg(path.c_str());

		ACL_VSTREAM *fp = acl_vstream_fopen(path.c_str(), O_RDONLY, 0700, 8192);
		char buf[1024];
		int ret = 0;
		acl::string cfg;
		while (ret != ACL_VSTREAM_EOF) {
			ret = acl_vstream_gets_nonl(fp, buf, sizeof(buf));
			cfg += buf;
		}
		acl_vstream_fclose(fp);

		acl::json json;
		json.update(cfg);
		acl::json_node* iter = json.first_node();
		while (iter)
		{
			if (iter->tag_name())
			{
				m_cfg.insert(std::make_pair(iter->tag_name(), iter->get_text()));
			}
			iter = json.next_node();
		}
	}
}

std::string& config::get_config(const char * name)
{
	init();
	return m_cfg[name];
}

int config::get_int(const char * name)
{
	init();
	auto vl = m_cfg[name];
	return vl.empty() ? 0 : atoi(vl.c_str());
}


/* ��  �ܣ���ȡָ����������Ӧ�Ŀ�ִ�У�EXE���ļ�ȫ·��
* ��  ����hProcess - ���̾�����������PROCESS_QUERY_INFORMATION ����
PROCESS_QUERY_LIMITED_INFORMATION Ȩ��
*         sFilePath - ���̾��hProcess����Ӧ�Ŀ�ִ���ļ�·��
* ����ֵ��
*/
void GetProcessFilePath(OUT string& sFilePath)
{
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
}