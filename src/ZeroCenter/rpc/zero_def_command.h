#ifndef ZERO_DEFAULT_COMMAND_H
#define ZERO_DEFAULT_COMMAND_H
#pragma once
namespace agebull
{
	namespace zero_net
	{
		namespace zero_def
		{
			/**
			 * \brief ����ʱ�Ŀ������:˵��֡�ĵڶ�����([1])
			 */
			namespace command
			{
				typedef unsigned char uchar;
				/**
				* \brief ������˵��
				*/
				constexpr uchar none = uchar('\1');

				/**
				* \brief ����ƻ�
				*/
				constexpr uchar plan = uchar('\2');

				/**
				* \brief ����ִ��
				*/
				constexpr uchar proxy = uchar('\3');
				/**
				* \brief ȡ��δ��������
				*/
				constexpr uchar restart = uchar('\4');

				/**
				* \brief ȡȫ�ֱ�ʶ
				*/
				constexpr uchar  global_id = uchar('>');

				/**
				* \brief �ȴ����
				*/
				constexpr uchar waiting = uchar('#');

				/**
				* \brief ���ҽ��
				*/
				constexpr uchar find_result = uchar('%');

				/**
				* \brief �رս��
				*/
				constexpr uchar close_request = uchar('-');

				/**
				* \brief Ping
				*/
				constexpr uchar ping = uchar('*');

				/**
				* \brief ��������
				*/
				constexpr uchar  heart_join = uchar('J');

				/**
				* \brief �����Ѿ���
				*/
				constexpr uchar  heart_ready = uchar('R');

				/**
				* \brief ��������
				*/
				constexpr uchar  heart_pitpat = uchar('P');

				/**
				* \brief �����˳�
				*/
				constexpr uchar  heart_left = uchar('L');

			}
		}
	}
}
#endif