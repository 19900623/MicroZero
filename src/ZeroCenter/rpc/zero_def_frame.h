#ifndef ZERO_DEF_FRAME_H
#define ZERO_DEF_FRAME_H
#pragma once

namespace agebull
{
	namespace zero_net
	{
		namespace zero_def
		{
			/**
			 * \brief ֡����˵������
			*/
			namespace frame
			{
				typedef unsigned char uchar;
				//��ֹ����
				constexpr uchar  end = '\0';
				//ȫ�ֱ�ʶ
				constexpr uchar  global_id = '\001';
				//վ��
				constexpr uchar  station_id = '\002';
				//״̬
				constexpr uchar  status = '\003';
				//����ID
				constexpr uchar  request_id = '\004';
				//ִ�мƻ�
				constexpr uchar  plan = '\005';
				//ִ�мƻ�
				constexpr uchar  plan_time = '\006';
				//������֤��ʶ
				constexpr uchar  service_key = '\007';
				//���ر�ʶ
				constexpr uchar  local_id = '\010';
				//ԭ������
				constexpr uchar  original_1 = '\011';
				//ԭ������
				constexpr uchar  original_2 = '\012';
				//ԭ������
				constexpr uchar  original_3 = '\013';
				//ԭ������
				constexpr uchar  original_4 = '\014';
				//ԭ������
				constexpr uchar  original_5 = '\015';
				//ԭ������
				constexpr uchar  original_6 = '\016';
				//ԭ������
				constexpr uchar  original_7 = '\017';
				//ԭ������
				constexpr uchar  original_8 = '\020';
				//����
				constexpr uchar  command = '$';
				//����
				constexpr uchar  arg = '%';
				//֪ͨ����
				constexpr uchar  pub_title = '*';
				//֪ͨ����
				constexpr uchar  sub_title = command;
				//������������Ϣ
				constexpr uchar  context = '#';
				//������/������
				constexpr uchar  requester = '>';
				//������/������
				constexpr uchar  publisher = requester;
				//�ظ���/�˷���
				constexpr uchar  responser = '<';
				//������/�˷���
				constexpr uchar  subscriber = responser;
				//����
				constexpr uchar  content = 'T';
				constexpr uchar  content_text = 'T';
				constexpr uchar  content_json = 'J';
				constexpr uchar  content_bin = 'B';
				constexpr uchar  content_xml = 'X';
				
			}
			/**
			* \brief ˵��֡����
			*/
			acl::string desc_str(bool in, char* desc, size_t len);
		}
	}
}
#endif