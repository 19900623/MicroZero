#pragma once
#ifndef AGEBULL_COMMAND_H_
#define  AGEBULL_COMMAND_H_
namespace agebull
{
	namespace zmq_net
	{
		//����״̬
#define  zero_status_success '+'
		//����״̬
#define zero_status_bad  '-'
//����״̬
#define zero_command_ok  "+ok"
#define zero_command_plan  "+plan"
#define zero_command_runing  "+runing"
#define zero_command_bye  "+bye"
#define zero_command_wecome  "+wecome"
#define zero_vote_sended  "+send"
#define zero_vote_closed  "+close"
#define zero_vote_bye  "+bye"
#define zero_vote_waiting  "+waiting"
#define zero_vote_start  "+start"
#define zero_vote_end  "+end"

#define zero_command_no_find  "-no find"
#define zero_command_invalid  "-invalid" 
#define zero_command_timeout  "-time out"
#define zero_command_net_error  "-net error"
#define zero_command_no_support  "-no support"
#define zero_command_failed  "-failed"
#define zero_command_error  "-error"

#define zero_command_arg_error  "-ArgumentError! must like : call[name][command][argument]"
#define zero_command_install_arg_error  "-ArgumentError! must like :install [type] [name]"
#define zero_api_unknow_error  "-error"
#define zero_api_not_worker  "-not work"
#define zero_vote_unknow_error  "-error"

#define zero_command_name_worker_join  '@'
#define zero_command_name_start  '*'

//��ֹ����
#define zero_frame_end  'E'
//ִ�мƻ�
#define zero_frame_plan  'P'
//����
#define zero_frame_arg  'A'
//����ID
#define zero_frame_request_id  'I'
//������/������
#define zero_frame_requester  'R'
//������/������
#define zero_frame_publisher  zero_frame_requester
//�ظ���/�˷���
#define zero_frame_responser  'G'
//������/�˷���
#define zero_frame_subscriber  zero_responser
//�㲥����
//#define zero_pub_title  '*'
//�㲥����
#define zero_frame_subtitle  'S'
//������������Ϣ
#define zero_frame_context  'T'
	}
}
#endif