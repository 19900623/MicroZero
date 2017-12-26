#ifndef _COMMAND_DEF_H
#define _COMMAND_DEF_H

#pragma once



//����״̬
typedef int NET_STATE;
const NET_STATE NET_STATE_NONE = 0;
const NET_STATE NET_STATE_RUNING = 1;
const NET_STATE NET_STATE_CLOSING = 2;
const NET_STATE NET_STATE_CLOSED = 3;
const NET_STATE NET_STATE_DISTORY = 4;

typedef void* ZMQ_HANDLE;

typedef char* TSON_BUFFER;

//��������
typedef unsigned short NET_COMMAND;

//ϵͳ֪ͨ
const NET_COMMAND NET_COMMAND_SYSTEM_NOTIFY = NET_COMMAND(0x1);
//ҵ��֪ͨ
const NET_COMMAND NET_COMMAND_BUSINESS_NOTIFY = NET_COMMAND(0x2);
//��������
const NET_COMMAND NET_COMMAND_CALL = NET_COMMAND(0x3);
//�����
const NET_COMMAND NET_COMMAND_RESULT = NET_COMMAND(0x4);
//���̴�����ʼ
const NET_COMMAND NET_COMMAND_FLOW_START = NET_COMMAND(0x10);
//���̽ڵ㴦��ɹ�
const NET_COMMAND NET_COMMAND_FLOW_STEP_SUCCEESS = NET_COMMAND(0x11);
//���̽ڵ㴦��ɹ�
const NET_COMMAND NET_COMMAND_FLOW_STEP_FAILED = NET_COMMAND(0x12);
//���̽ڵ㴦��״̬��ȡ
const NET_COMMAND NET_COMMAND_FLOW_STTATE = NET_COMMAND(0x13);
//���̴������
const NET_COMMAND NET_COMMAND_FLOW_END = NET_COMMAND(0x17);

//��������״̬ -0x10֮��Ϊ�Զ������
typedef int COMMAND_STATE;
//����
const COMMAND_STATE NET_COMMAND_STATE_DATA = 0x0;
//�������
const COMMAND_STATE NET_COMMAND_STATE_SENDING = 0x7;
//�����ѷ���
const COMMAND_STATE NET_COMMAND_STATE_SENDED = 0x8;
//�������ڷ�����Ŷ�
const COMMAND_STATE NET_COMMAND_STATE_WAITING = 0x9;
//������ִ�����
const COMMAND_STATE NET_COMMAND_STATE_SUCCEED = 0xA;
//����ͳ���
const COMMAND_STATE NET_COMMAND_STATE_NETERROR = 0x6;
//δ֪����(δ�յ���ִ)
const COMMAND_STATE NET_COMMAND_STATE_UNKNOW = 0x5;
//������δ֪����(ϵͳ�쳣)
const COMMAND_STATE NET_COMMAND_STATE_SERVER_UNKNOW = 0x4;
//����δ֪����(ϵͳ�쳣)
const COMMAND_STATE NET_COMMAND_STATE_CLIENT_UNKNOW = 0x3;
//�����ظ�����
const COMMAND_STATE NET_COMMAND_STATE_DATA_REPEAT = 0x2;
//�������ִ��
const COMMAND_STATE NET_COMMAND_STATE_CANNOT = 0x1;

//��������
const COMMAND_STATE NET_COMMAND_STATE_ARGUMENT_INVALID = -1;
//�߼�����
const COMMAND_STATE NET_COMMAND_STATE_LOGICAL_ERROR = -2;


#define GUID_LEN 34 

//�����������ò����ṹ
typedef struct {
	char user_token[GUID_LEN];//�û���ʶ(�û���ʶ�û���ͬʱҲ��0MQ�ķ�����ɸѡ��)
	char cmd_identity[GUID_LEN];//�����ʶ(�����첽�ط��������״̬��Ӧ)
	NET_COMMAND cmd_id;//����(���÷�����)
	unsigned short try_num;//����(�ط�����)
	COMMAND_STATE cmd_state;//����״̬(0Ϊ����)
	size_t crc_code;//����ͷ��CRC16У����
	size_t data_len;//��������--�˴�ΪTSON��ͷ��
}NetCommand, *PNetCommand;


//���������ͷ����(������CRC�ֶ�)
#define NETCOMMAND_BODY_LEN sizeof(NetCommand) - sizeof(size_t) - sizeof(size_t)
//���������ͷ����
#define NETCOMMAND_HEAD_LEN sizeof(NetCommand) - sizeof(size_t)
//ȡ�������������ݵ�ַ
#define get_cmd_buffer(cmd) (TSON_BUFFER)(((char*)cmd) + sizeof(NetCommand) - sizeof(size_t))
//ȡ������������ʵ�ڴ泤��
#define get_cmd_len(cmd_call) cmd_call->data_len == 0 ? sizeof(NetCommand) : cmd_call->data_len + sizeof(NetCommand) - sizeof(size_t)
//ȡ������������������
#define get_cmd_data_type(cmd_call) (*((int*)(((char*)cmd_call) + sizeof(NetCommand))))

//��ʾһ���������ָ��,���ƹ�����ָ��ᶪʧ
class NetCommandArgPtr
{
	int* m_control;
public:
	char* m_buffer;
	size_t m_len;
	PNetCommand m_command;
	NetCommandArgPtr()
		: m_control(nullptr)
		, m_buffer(nullptr)
		, m_len(0)
		, m_command(nullptr)
	{
	}
	NetCommandArgPtr(NetCommandArgPtr& ptr)
		: m_control(ptr.m_control)
		, m_buffer(ptr.m_buffer)
		, m_len(ptr.m_len)
		, m_command(ptr.m_command)
	{
		*m_control += 1;
	}
	NetCommandArgPtr(size_t len)
		: m_control(new int)
		, m_buffer(new char[len])
		, m_len(len)
	{
		m_command = reinterpret_cast<PNetCommand>(m_buffer);
		*m_control = 1;
	}
	NetCommandArgPtr(PNetCommand cmd)
		: m_control(new int)
		, m_buffer(reinterpret_cast<char*>(cmd))
		, m_len(get_cmd_len(cmd))
		, m_command(cmd)
	{
		*m_control = 1;
	}
	NetCommandArgPtr(char* buffer, size_t len)
		: m_control(new int)
		, m_buffer(buffer)
		, m_len(len)
		, m_command(reinterpret_cast<PNetCommand>(buffer))
	{
		*m_control = 1;
	}
	~NetCommandArgPtr()
	{
		if (m_control == nullptr || (*m_control) < 0)//С��0��ʾ������ָ��������
			return;
		(*m_control) -= 1;
		if (*m_control == 0)
		{
			(*m_control) = -1;
			delete m_control;
			delete[] m_buffer;
		}
	}
	NetCommandArgPtr& operator=(NetCommandArgPtr& right)
	{
		m_buffer = right.m_buffer;
		m_command = right.m_command;
		m_len = right.m_len;
		m_control = right.m_control;
		if (m_control != nullptr)
			*m_control += 1;
		return *this;
	}
	//NetCommandArgPtr& operator=(PNetCommand arg)
	//{
	//	m_buffer = reinterpret_cast<char*>(arg);
	//	m_command = arg;
	//	m_len = get_cmd_len(arg);
	//	*m_control = 1;
	//	return *this;
	//}
	PNetCommand& operator->()
	{
		return m_command;
	}
	PNetCommand& operator*()
	{
		return m_command;
	}
};

#endif // !_COMMAND_DEF_H