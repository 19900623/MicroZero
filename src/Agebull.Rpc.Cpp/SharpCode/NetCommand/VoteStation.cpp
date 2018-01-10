/**
 * ZMQ�㲥������
 *
 *
 */

#include "stdafx.h"
#include "VoteStation.h"
#include <zeromq/zhelpers.h>

namespace agebull
{
	namespace zmq_net
	{


		bool VoteStation::get_voters(const char* client_addr, const char* request_token)
		{
			acl::string strs("[");
			bool first = true;
			for (auto iter : _workers)
			{
				if (!first)
				{
					strs.append("\"");
					first = false;
				}
				else
					strs.append(",\"");
				strs.append(iter.second.flow_name.c_str());
				strs.append("\"");
			}
			strs.append("]");
			return send_state(client_addr, request_token, "*", strs.c_str());
		}

		/**
		* @brief ִ��һ������
		*/
		sharp_char VoteStation::command(const char* caller, vector<string> lines)
		{
			if (send_state(caller, lines[0].c_str(), lines[1].c_str(), lines[2].c_str()))
				return sharp_char("vote start");
			return sharp_char("bad");
		}

		/**
		* @brief ��Զ�̵��ý���ʱ�Ĵ���
		*/
		void VoteStation::request(ZMQ_HANDLE socket)
		{
			vector<sharp_char> list;
			//0 ·�ɵ��ĵ�ַ 1 ��֡ 2 �����ʶ 3 ���� 4 ����
			_zmq_state = recv(_out_socket, list);

			RedisLiveScope redis_live_scope;
			switch (list[3][0])
			{
			case '*': //����ͶƱ������
				get_voters(*list[0], *list[2]);
				break;
			case '@': //��ʼͶƱ
				TransRedis::get_context()->set(*list[2], "start");
				TransRedis::get_context()->pexpire(*list[2], 3600000);//3600�����
				if (start_vote(*list[0], *list[3], *list[4]))
					send_state(*list[0], *list[2], "*", "start");
				else
					send_state(*list[0], *list[2], "*", "error");
				break;
			case '%': //�����ȴ�ͶƱ����
				TransRedis::get_context()->set(*list[2], "waiting");
				TransRedis::get_context()->pexpire(*list[2], 3600000);//3600�����
				send_state(*list[0], *list[2], "*", "waiting");
				break;
			case 'v': //����ͶƱ
				TransRedis::get_context()->set(*list[2], "end");
				send_state(*list[0], *list[2], "*", "end");
				break;
			case 'x': //����ͶƱ
				TransRedis::get_context()->set(*list[2], "close");
				send_state(*list[0], *list[2], "*", "close");
				break;
			default:
				send_state(*list[0], *list[2], "*", "error");
				break;
			}
		}

		bool VoteStation::start_vote(const char* client_addr, const char* request_token, const char* request_argument)
		{
			//·�ɵ����й�������
			for (auto voter : _workers)
			{
				_zmq_state = send_addr(_inner_socket, voter.second.net_name.c_str());
				_zmq_state = send_more(_inner_socket, client_addr);
				_zmq_state = send_more(_inner_socket, request_token);
				_zmq_state = send_late(_inner_socket, request_argument);
			}
			return _zmq_state == ZmqSocketState::Succeed;
		}

		bool VoteStation::send_state(const char* client_addr, const char* request_token, const char* voter, const char* state)
		{
			_zmq_state = send_addr(_out_socket, client_addr);
			_zmq_state = send_more(_out_socket, request_token);
			_zmq_state = send_more(_out_socket, voter);
			_zmq_state = send_late(_out_socket, state);
			return _zmq_state == ZmqSocketState::Succeed;
		}

		/**
		* @brief ��������������ʱ�Ĵ���
		*/
		void VoteStation::response()
		{
			vector<sharp_char> list;
			//0 ·�ɵ��ĵ�ַ 1 ��֡ 2 ԭʼ�������ַ 3 �����ʶ 4 ���
			_zmq_state = recv(_inner_socket, list);
			if (_zmq_state == ZmqSocketState::TimedOut)
			{
				return;
			}
			if (_zmq_state != ZmqSocketState::Succeed)
			{
				log_error3("���ս��ʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}

			switch (list[2][0])
			{
			case '@'://����
				worker_join(*list[0], *list[3], true);
				send_addr(_inner_socket, *list[0]);
				send_late(_inner_socket, "wecome");
				return;
			case '*'://��ʼ����
				return;
			default: break;
			}
			RedisLiveScope redis_live_scope;
			if (TransRedis::get_context()->exists(*list[3]))//���ͶƱ����Ч�����ؽ�ͶƱ��
			{
				auto vote = _workers[*list[0]];
				acl::string ver("|");
				ver.append(vote.flow_name.c_str());
				TransRedis::get_context()->append(*list[3], ver.c_str());
				send_state(*list[0], *list[3], vote.flow_name.c_str(), *list[4]);
			}
		}
	}
}
