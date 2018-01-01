#pragma once
#ifndef AGEBULL_CONFIG_H
#include <stdinc.h>
class config
{
	static std::map<std::string, std::string> m_machine_cfg;
	static void init();
public:
	static int get_int(const char * name);
	static std::string& get_config(const char * name);
private:
	std::map<std::string, std::string> m_cfg;
	static void read(acl::string& str, std::map<std::string, std::string>& cfg);
public:
	config(const char* json);
	int number(const char * name);
	bool boolean(const char * name);
	std::string& operator[](const char * name);
};

/* ��  �ܣ���ȡָ����������Ӧ�Ŀ�ִ�У�EXE���ļ�ȫ·��
* ��  ����hProcess - ���̾�����������PROCESS_QUERY_INFORMATION ����
PROCESS_QUERY_LIMITED_INFORMATION Ȩ��
*         sFilePath - ���̾��hProcess����Ӧ�Ŀ�ִ���ļ�·��
* ����ֵ��
*/
void GetProcessFilePath(OUT string& sFilePath);
#endif