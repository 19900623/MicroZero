#include "stdafx.h"
#include "ZeroStation.h"

namespace agebull
{
	namespace zmq_net
	{

		/**
		* \brief �ƻ���ѯ
		*/
		void zero_station::plan_poll()
		{
			in_plan_poll_ = true;
			while (can_do())
			{
				bool doit = true;
				for (int i = 0; i < 60; i++)
				{
					sleep(1);
					if (!can_do())
					{
						doit = false;
						break;
					}
				}
				if (!doit)
					break;
				vector<plan_message> messages;
				load_now(messages);
				for (plan_message msg : messages)
				{
					command(msg.request_caller.c_str(), msg.messages);
					if (zmq_state_ == zmq_socket_state::Succeed)
					{
						plan_next(msg, false);
					}
				}
			}
			in_plan_poll_ = false;
		}


		/**
		* \brief �������ڵ��ڵ�����
		*/
		void zero_station::load_now(vector<plan_message>& messages) const
		{
			char zkey[100];
			sprintf_s(zkey, "zero:plan:%s", station_name_.c_str());
			vector<acl::string> keys;
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			trans_redis::get_context()->zrangebyscore(zkey, 0, static_cast<double>(time(nullptr)), &keys);
			for (const acl::string& key : keys)
			{
				plan_message message;
				load_message(static_cast<uint>(acl_atoll(key.c_str())), message);
				messages.push_back(message);
			}
		}

		/**
		* \brief ɾ��һ���ƻ�
		*/

		bool zero_station::remove_next(plan_message& message) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			char zkey[MAX_PATH];
			sprintf_s(zkey, "zero:plan:%s", station_name_.c_str());
			char id[MAX_PATH];
			_i64toa_s(message.plan_id, id, MAX_PATH, 10);
			return trans_redis::get_context()->zrem(zkey, id) >= 0;
		}


		/**
		 * \brief �ƻ���һ��ִ��ʱ��
		 * \param message 
		 * \param first 
		 * \return 
		 */
		bool zero_station::plan_next(plan_message& message, const bool first) const
		{
			if (!first && message.plan_repet >= 0 && message.real_repet >= message.plan_repet)
			{
				remove_message(message);
				remove_next(message);
				return false;
			}
			if (!first)
				message.real_repet += 1;
			save_message(message);
			save_next(message);
			return true;
		}


		/**
		* \brief ������һ��ִ��ʱ��
		*/
		bool zero_station::save_next(plan_message& message) const
		{
			time_t t = time(nullptr);
			switch (message.plan_type)
			{
			case plan_date_type::time:
				t = message.plan_value;
				break;
			case plan_date_type::minute:
				t += message.plan_value * 60;
				break;
			case plan_date_type::hour:
				t += message.plan_value * 3600;
				break;
			case plan_date_type::day:
				t += message.plan_value * 24 * 3600;
				break;
			default: return false;
			}
			char zkey[MAX_PATH];
			sprintf_s(zkey, "msg:time:%s", station_name_.c_str());

			char id[MAX_PATH];
			_i64toa_s(message.plan_id, id, MAX_PATH,10);
			map<acl::string, double> value;
			value.insert(make_pair(id, static_cast<double>(t)));

			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			{
				return trans_redis::get_context()->zadd(zkey, value) >= 0;
			}
		}

		/**
		* \brief ������Ϣ
		*/

		bool zero_station::save_message(plan_message& message) const
		{
			char key[MAX_PATH];
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			if (message.plan_id == 0)
			{
				sprintf_s(key, "zero:identity:%s", station_name_.c_str());
				message.plan_id = static_cast<uint32_t>(trans_redis::get_context().incr_redis(key)) + 1;
			}
			sprintf_s(key, "zero:message:%s:%8x", station_name_.c_str(), message.plan_id);
			return trans_redis::get_context()->set(key, message.write_json().c_str());
		}

		/**
		* \brief ��ȡ��Ϣ
		*/

		bool zero_station::load_message(uint id, plan_message& message) const
		{
			char key[MAX_PATH];
			sprintf_s(key, "zero:message:%s:%8x", station_name_.c_str(), id);
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			acl::string val;
			if (!trans_redis::get_context()->get(key, val) || val.empty())
			{
				return false;
			}
			message.read_json(val);
			
			return true;
		}


		/**
		* \brief ɾ��һ����Ϣ
		*/

		bool zero_station::remove_message(plan_message& message) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			trans_redis& redis = trans_redis::get_context();
			char key[MAX_PATH];
			char id[MAX_PATH];
			_itoa_s(message.plan_id, id, MAX_PATH);
			//1 ɾ����Ϣ
			sprintf_s(key, "zero:message:%s:%8x", station_name_.c_str(), message.plan_id);
			redis->del(key);
			//2 ɾ���ƻ�
			if (message.plan_type > plan_date_type::none)
			{
				sprintf_s(key, "zero:plan:%s", station_name_.c_str());
				redis->zrem(key, id);
			}
			//3 ɾ��������
			sprintf_s(key, "zero:worker:%s:%8x", station_name_.c_str(), message.plan_id);
			acl::string val;
			while (redis->spop(key, val))
			{
				sprintf_s(key, "zero:request:%s:%s", station_name_.c_str(), val.c_str());
				redis->srem(key, id);
			}
			redis->del(key);
			//4 ɾ������ֵ
			sprintf_s(key, "zero:result:%s:%8x", station_name_.c_str(), message.plan_id);
			redis->del(key);
			return true;
		}

		/**
		* \brief ������Ϣ������
		*/

		bool zero_station::save_message_worker(uint msgid, vector<const char*>& workers) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);
			char key[MAX_PATH];
			sprintf_s(key, "zero:worker:%s:%8x", station_name_.c_str(), msgid);
			trans_redis::get_context()->sadd(key, workers);
			char id[MAX_PATH];
			_itoa_s(msgid, id, MAX_PATH);
			for (auto work : workers)
			{
				sprintf_s(key, "zero:request:%s:%s", station_name_.c_str(), work);
				trans_redis::get_context()->sadd(key, id);
			}
			return true;
		}

		/**
		* \brief ������Ϣ�����߷���ֵ
		*/

		bool zero_station::save_message_result(uint msgid, const string& worker, const string& response) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);

			char key[MAX_PATH];
			sprintf_s(key, "zero:worker:%s:%8x", station_name_.c_str(), msgid);
			trans_redis::get_context()->srem(key, worker.c_str());

			char id[MAX_PATH];
			_itoa_s(msgid, id, MAX_PATH);
			sprintf_s(key, "zero:request:%s:%s", station_name_.c_str(), worker.c_str());
			trans_redis::get_context()->srem(key, id);

			sprintf_s(key, "zero:result:%s:%8x", station_name_.c_str(), msgid);
			trans_redis::get_context()->hset(key, worker.c_str(), response.c_str());
			return true;
		}

		/**
		* \brief ȡһ�������ߵ���Ϣ����ֵ
		*/

		acl::string zero_station::get_message_result(uint msgid, const char* worker) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);

			char key[MAX_PATH];

			sprintf_s(key, "zero:result:%s:%8x", station_name_.c_str(), msgid);
			acl::string val;
			trans_redis::get_context()->hget(key, worker, val);
			return val;
		}

		/**
		* \brief ȡȫ����������Ϣ����ֵ
		*/

		map<acl::string, acl::string> zero_station::get_message_result(uint msgid) const
		{
			redis_live_scope redis_live_scope;
			redis_db_scope db_scope(REDIS_DB_ZERO_MESSAGE);

			char key[MAX_PATH];
			map<acl::string, acl::string> result;
			sprintf_s(key, "zero:result:%s:%8x", station_name_.c_str(), msgid);
			trans_redis::get_context()->hgetall(key, result);
			return result;
		}
	}
}
