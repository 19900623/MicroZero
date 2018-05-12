#pragma once
#ifndef AGEBULL_COMMAND_H_
#define  AGEBULL_COMMAND_H_
namespace agebull
{
	namespace zmq_net
	{
		//����״̬
#define  ZERO_STATUS_SUCCESS '+'
		//����״̬
#define ZERO_STATUS_BAD  '-'
//����״̬
#define ZERO_STATUS_OK  "+ok"
#define ZERO_STATUS_PLAN  "+plan"
#define ZERO_STATUS_RUNING  "+runing"
#define ZERO_STATUS_BYE  "+bye"
#define ZERO_STATUS_WECOME  "+wecome"
#define ZERO_STATUS_VOTE_SENDED  "+send"
#define ZERO_STATUS_VOTE_CLOSED  "+close"
#define ZERO_STATUS_VOTE_BYE  "+bye"
#define ZERO_STATUS_VOTE_WAITING  "+waiting"
#define ZERO_STATUS_VOTE_START  "+start"
#define ZERO_STATUS_VOTE_END  "+end"

#define ZERO_STATUS_NO_FIND  "-no find"
#define ZERO_STATUS_FRAME_INVALID  "-invalid" 
#define ZERO_STATUS_TIMEOUT  "-time out"
#define ZERO_STATUS_NET_ERROR  "-net error"
#define ZERO_STATUS_NO_SUPPORT  "-no support"
#define ZERO_STATUS_FAILED  "-failed"
#define ZERO_STATUS_ERROR  "-error"
#define ZERO_STATUS_API_NOT_WORKER  "-not work"

#define ZERO_STATUS_MANAGE_ARG_ERROR  "-ArgumentError! must like : call[name][command][argument]"
#define ZERO_STATUS_MANAGE_INSTALL_ARG_ERROR  "-ArgumentError! must like :install [type] [name]"


//��ֹ����
#define ZERO_FRAME_END  'E'
//ִ�мƻ�
#define ZERO_FRAME_PLAN  'P'
//����
#define ZERO_FRAME_ARG  'A'
//����
#define ZERO_FRAME_COMMAND  'C'
//����ID
#define ZERO_FRAME_REQUEST_ID  'I'
//������/������
#define ZERO_FRAME_REQUESTER  'R'
//������/������
#define ZERO_FRAME_PUBLISHER  ZERO_FRAME_REQUESTER
//�ظ���/�˷���
#define ZERO_FRAME_RESPONSER  'G'
//������/�˷���
#define ZERO_FRAME_SUBSCRIBER  zero_responser
//�㲥����
//#define zero_pub_title  '*'
//�㲥����
#define ZERO_FRAME_SUBTITLE  'S'
//������������Ϣ
#define ZERO_FRAME_CONTEXT  'T'
//״̬
#define ZERO_FRAME_STATUS  'S'




/**
* \brief ����վ�����
*/
#define  ZERO_WORKER_JOIN  '@' 
/**
* \brief ����վ�����
*/
#define  ZERO_WORKER_LEFT  '!' 
/**
* \brief ����վ��ȴ�����
*/
#define  ZERO_WORKER_LISTEN  '$' 

/**
* \brief ��������
*/
#define  ZERO_HEART_JOIN  '@' 

/**
* \brief ��������
*/
#define  ZERO_HEART_PITPAT  '$' 

/**
* \brief �����˳�
*/
#define  ZERO_HEART_LEFT  '!' 

	}
}
#endif