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
		VoteStation::VoteStation(string name)
			: RouteStation<VoteStation, Voter, STATION_TYPE_VOTE>(name)
		{
		}

		bool  VoteStation::get_voters(const char* client_addr, const  char* request_token)
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
		bool  VoteStation::send_state(const char* client_addr, const  char* request_token, const  char* voter, const  char* state)
		{
			_zmq_state = s_sendmore(_outSocket, client_addr);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_outSocket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_outSocket, request_token);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_outSocket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_outSocket, voter);
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_sendmore(_outSocket, "");
			if (_zmq_state < 0)
				return false;
			_zmq_state = s_send(_outSocket, state);//��ʵ����
			return _zmq_state > 0;
		}
		/**
		* ��Զ�̵��ý���ʱ�Ĵ���
		*/
		void VoteStation::onCallerPollIn()
		{
			// ��ȡ��һ��client�����󣬽������е�worker����
			// client�������Ϣ��ʽ�ǣ�[client��ַ][��֡][��������]
			char* client_addr = s_recv(_outSocket, 0);
			recv_empty(_outSocket);
			char* request_token = s_recv(_outSocket);
			recv_empty(_outSocket);
			char* request_state = s_recv(_outSocket);
			recv_empty(_outSocket);
			char* request_argument = s_recv(_outSocket);
			RedisLiveScope redis_live_scope;
			if (strcmp(request_state, "voters"))//����ͶƱ������
			{
				TransRedis::get_context()->set(request_token, "*");//3600�����
				TransRedis::get_context()->pexpire(request_token, 3600000);//3600�����
				get_voters(client_addr, request_argument);
			}
			else if (strcmp(request_state, "start"))//��ʼͶƱ
			{
				TransRedis::get_context()->set(request_token, "#");//3600�����
				start_vote(client_addr, request_token, request_argument);
			}
			else if (strcmp(request_state, "continue"))//�����ȴ�ͶƱ����
			{
				TransRedis::get_context()->pexpire(request_token, 3600000);//3600�����
			}
			else if (strcmp(request_state, "end") || strcmp(request_state, "bye"))//����ͶƱ
			{
				delete_from_redis(request_token);
				send_state(client_addr, request_token, "*", "bye");
			}
			else
			{
				send_state(client_addr, request_token, "*", "fuck");
			}

			free(client_addr);
			free(request_token);
			free(request_argument);
		}

		bool VoteStation::start_vote(const char* client_addr, const  char* request_token, const  char* request_argument)
		{
			//·�ɵ����й�������
			for (auto voter : _workers)
			{
				_zmq_state = s_sendmore(_innerSocket, voter.second.net_name.c_str());
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_sendmore(_innerSocket, "");
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_sendmore(_innerSocket, client_addr);
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_sendmore(_innerSocket, "");
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_send(_innerSocket, request_token);//��ʵ����
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_sendmore(_innerSocket, "");
				if (_zmq_state < 0)
					return false;
				_zmq_state = s_send(_innerSocket, request_argument);//��ʵ����
				if (_zmq_state < 0)
					return false;
			}
			return true;
		}
		/**
		* ��������������ʱ�Ĵ���
		*/
		void VoteStation::onWorkerPollIn()
		{
			// ��worker�ĵ�ַ���
			char* worker_addr = s_recv(_innerSocket);
			recv_empty(_innerSocket);
			char* client_addr = s_recv(_innerSocket);
			recv_empty(_innerSocket);
			char* request_token = s_recv(_outSocket);
			recv_empty(_outSocket);
			char* reply = s_recv(_innerSocket);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(request_token, "READY") == 0)
			{
				join(worker_addr, reply, true);
			}
			else
			{
				RedisLiveScope redis_live_scope;
				if (TransRedis::get_context()->exists(request_token))//���ͶƱ����Ч�����ؽ�ͶƱ��
				{
					auto vote = _workers[worker_addr];
					acl::string ver("|");
					ver.append(vote.flow_name.c_str());
					TransRedis::get_context()->append(request_token, ver.c_str());
					send_state(client_addr, request_token, vote.flow_name.c_str(), reply);
				}
			}
			free(worker_addr);
			free(client_addr);
			free(request_token);
			free(reply);
		}
	}
}