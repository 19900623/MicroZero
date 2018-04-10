#include "transredis.h"
#include "cfg/config.h"
namespace agebull
{
	/**
	* \brief ��ǰ�߳̾�̬Ψһ
	*/
	static __thread  trans_redis* _context = nullptr;

	/**
	* \brief �����ļ��е�redis�ĵ�ַ
	*/
	string RedisIp = config::get_config("redis_add");
	/**
	* \brief �����ļ��е�redis��db
	*/
	int RedisDb = config::get_int("redis_defdb");

	/**
	* \brief ����
	*/
	redis_db_scope::~redis_db_scope()
	{
		redis->select(RedisDb);
	}
	/**
	* \brief ���ɵ�ǰ�߳������ĵ�����Redis����
	*/
	bool trans_redis::open_context()
	{
		const bool is_new = _context == nullptr;
		if (is_new)
		{
			RedisIp = config::get_config("redis_add");
			RedisDb = config::get_int("redis_defdb");
			_context = new trans_redis();
#if _DEBUG
			_context->m_last_status = _context->m_redis_cmd->ping();
#endif
			(*_context)->select(RedisDb);
		}
#if _DEBUG
		else
		{
			_context->m_last_status = _context->m_redis_cmd->ping();
		}
#endif
		return is_new;
	}
	/**
	* \brief ���ɵ�ǰ�߳������ĵ�����Redis����
	*/
	bool trans_redis::open_context(int db)
	{
		const bool is_new = _context == nullptr;
		if (is_new)
		{
			RedisIp = config::get_config("redis_add");
			RedisDb = config::get_int("redis_defdb");
			_context = new trans_redis();
#if _DEBUG
			_context->m_last_status = _context->m_redis_cmd->ping();
#endif
			(*_context)->select(db);
		}
		else
		{
#if _DEBUG
			(*_context)->select(db);
			_context->m_last_status = _context->m_redis_cmd->ping();
#endif
		}
		return is_new;
	}
	/**
	* \brief ȡ�õ�ǰ�߳������ĵ�����Redis����
	* @return ��ǰ�߳������ĵĲ�������
	*/
	trans_redis& trans_redis::get_context()
	{
		open_context();
		return *_context;
	}
	/**
	* \brief ȡ�õ�ǰ�߳������ĵ�����Redis����
	* @return ��ǰ�߳������ĵĲ�������
	*/
	trans_redis* trans_redis::get_current()
	{
		return _context;
	}
	/**
	* \brief �رյ�ǰ�߳������ĵ�����Redis����
	*/
	void trans_redis::close_context()
	{
		if (_context != nullptr)
		{
			delete _context;
			_context = nullptr;
		}
	}
	/**
	* \brief ����
	*/
	trans_redis::trans_redis()
		: m_trans_num(0)
		, m_failed(false)
		, m_last_status(true)
	{
		m_redis_client = new acl::redis_client(RedisIp.c_str(), 0, 0);
		m_redis_cmd = new acl::redis(m_redis_client);
		m_redis_cmd->select(RedisDb);
	}
	/**
	* \brief ����
	*/
	trans_redis::~trans_redis()
	{
		if (m_trans_num > 0)
		{
			m_trans_num = 0;
			if (!m_failed)
				commit_inner();
		}
		if (m_redis_cmd == nullptr)
			return;
		m_redis_cmd->quit();
		delete m_redis_cmd;
		m_redis_cmd = nullptr;
		m_redis_client->close();
		delete m_redis_client;
		m_redis_client = nullptr;
		if (_context == this)
			_context = nullptr;
	}

	/**
	* \brief ��������
	* @return ��ǰ�߳������ĵĲ�������
	*/
	trans_redis& trans_redis::begin_trans()
	{
		get_context();
		_context->m_trans_num += 1;
		return *_context;
	}
	/**
	* \brief �ύ����,�������������������ĵط�����,ֻ�Ǽ����������ô���,���һ�ε���(��Ӧ�������begin_trans)ʱ,���֮ǰm_failed������,�ڲ����ǻ����rollback,����ignore_failed����Ϊtrue
	* @param {bool} ignore_failed ����m_failed������,�����Եĵ����ύ
	*/
	void trans_redis::end_trans(bool ignore_failed)
	{
		log_debug(DEBUG_BASE, 5, "�ر�redis����");
		if (_context == nullptr)
			return;
		_context->m_trans_num--;
		if (_context->m_trans_num > 0)
			return;
		if (ignore_failed || !_context->m_failed)
			_context->commit_inner();
		delete _context;
		_context = nullptr;
	}
	/**
	* \brief ��������
	*/
	void trans_redis::commit_inner()
	{
		auto start = m_modifies.begin();
		const auto end = m_modifies.end();
		log_debug(DEBUG_BASE, 5, "��ʼ�ύredis����...");

		while (start != end)
		{
			acl::string&vl = m_local_values[start->first];
			if (!m_redis_cmd->set(start->first.c_str(), start->first.length(), vl.c_str(), vl.length()))
			{
				log_error3("(%s)write_to_redis(%s)ʱ��������(%s)", RedisIp.c_str(), start->first.c_str(), m_redis_cmd->result_error());
				break;
			}
			++start;
		}
		log_debug(DEBUG_BASE, 5, "�����ύredis����");
	}
	/**
	* \brief ���ó���
	*/
	void trans_redis::set_failed()
	{
		get_context().m_failed = true;
	}

	bool trans_redis::get(const char* key, acl::string& vl)
	{
		if (m_trans_num > 0 && !m_local_values.empty())
		{
			auto it = m_local_values.find(key);
			if (it != m_local_values.end())
			{
				vl = it->second;
				log_debug2(DEBUG_BASE, 5, "redis->get %s(%s)", key, vl.c_str());
				return true;
			}
		}
		m_redis_cmd->clear();
		acl::string vl2;
		if (!m_redis_cmd->get(key, vl2))
		{
			log_error3("(%s)read_from_redis(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			return false;
		}
		m_local_values[key] = vl2;
		vl = vl2;
		log_debug2(DEBUG_BASE, 5, "redis->get %s(%s)", key, vl.c_str());
		return true;
	}
	void trans_redis::set(const char* key, acl::string&vl)
	{
		log_debug2(DEBUG_BASE, 5, "redis->set %s(%s)", key, vl.c_str());

		if (m_trans_num > 0)
		{
			m_local_values[key] = vl;
			//m_modifies[key] = 1;
		}
		//else
		{
			if (!m_redis_cmd->set(key, strlen(key), vl.c_str(), vl.length()))
			{
				log_error3("(%s)write_to_redis(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			}
		}
	}
	void trans_redis::set(const char* key, const char* vl)
	{
		log_debug2(DEBUG_BASE, 5, "redis->set %s(%s)", key, vl);

		if (m_trans_num > 0)
		{
			m_local_values[key] = vl;
			//m_modifies[key] = 1;
		}
		//else
		{
			if (!m_redis_cmd->set(key, strlen(key), vl, strlen(vl)))
			{
				log_error3("(%s)write_to_redis(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			}
		}
	}
	void trans_redis::set(const char* key, const char* vl, size_t len)
	{
		log_debug2(DEBUG_BASE, 5, "redis->set %s(binary %d)", key, len);

		if (m_trans_num > 0)
		{
			acl::string svl;
			svl.set_bin(true);
			svl.copy(vl, len);
			m_local_values[key] = svl;
			//m_modifies[key] = 1;
		}
		//else
		{
			if (!m_redis_cmd->set(key, strlen(key), vl, len))
			{
				log_error3("(%s)write_to_redis(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			}
		}
	}
	acl::string trans_redis::read_str_from_redis(const char* key)
	{
		acl::string str;
		get(key, str);
		return str;
	}

	acl::string trans_redis::read_from_redis(const char* key)
	{
		acl::string str;
		str.set_bin(true);
		str.set_max(0x8000);
		get(key, str);
		return str;
	}
	acl::string trans_redis::read_first_from_redis(const char* key)
	{
		m_redis_cmd->clear();
		acl::string str;
		str.set_bin(true);
		vector<acl::string> keys;
		if (m_redis_cmd->keys_pattern(key, &keys) < 0)
		{
			log_error3("(%s)keys_pattern(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
		}
		if (!keys.empty())
		{
			if (!get(keys.begin()->c_str(), str))
			{
				log_error3("(%s)read_first_from_redis(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			}
		}
		return str;
	}
	void trans_redis::write_json_to_redis(const char* key, const char* json)
	{
		m_redis_cmd->clear();
		if (json[strlen(json) - 1] != '}')
		{
			log_error3("(%s)write_json_to_redis(%s)ʱ(%s)����JSON", RedisIp, key, json);
		}
		else
		{
			set(key, json);
		}
	}

	void trans_redis::write_to_redis(const char* key, const char* bin, size_t len)
	{
		acl::string vl;
		vl.set_bin(true);
		vl.copy(bin, len);
		set(key, vl);
	}
	vector<acl::string> trans_redis::find_redis_keys(const char* find_key)
	{
		vector<acl::string> keys;
		m_redis_cmd->clear();
		if (m_redis_cmd->keys_pattern(find_key, &keys) < 0)
		{
			log_error3("(%s)keys_pattern(%s)ʱ��������(%s)", RedisIp, find_key, m_redis_cmd->result_error());
		}
		return keys;
	}
	vector<acl::string*> trans_redis::find_from_redis(const char* find_key)
	{
		vector<acl::string*> values;
		vector<acl::string> keys;
		//m_redis_cmd->clear();
		if (m_redis_cmd->keys_pattern(find_key, &keys) < 0)
		{
			log_error3("(%s)keys_pattern(%s)ʱ��������(%s)", RedisIp, find_key, m_redis_cmd->result_error());
			return values;
		}
		if (!keys.empty())
		{
			for each(auto key in keys)
			{
				acl::string* str = new acl::string();
				if (get(key, *str) == false)
				{
					log_error3("(%s)get(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
					continue;
				}
				values.push_back(str);
			}
		}
		return values;
	}

	void trans_redis::delete_from_redis(const char* find_key)
	{
		vector<acl::string> keys;
		if (m_redis_cmd->keys_pattern(find_key, &keys) < 0)
		{
			log_error3("(%s)keys_pattern(%s)ʱ��������(%s)", RedisIp, find_key, m_redis_cmd->result_error());
		}
		if (!keys.empty())
		{
			for each(auto key in keys)
			{
				m_redis_cmd->clear();
				if (m_redis_cmd->del(key) < 0)
				{
					log_error3("(%s)del(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
				}
			}
		}
	}

	size_t trans_redis::incr_redis(const char* key)
	{
		long long id;
		redis_db_scope scope(REDIS_DB_ZERO_SYSTEM);
		if (!m_redis_cmd->incr(key, &id))
		{
			log_error3("(%s)incr(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			id = 0LL;
		}
		return static_cast<size_t>(id);
	}


	bool trans_redis::lock_from_redis(const char* key)
	{
		redis_db_scope scope(REDIS_DB_ZERO_SYSTEM);
		char vl[2] = "1";
		const int re = m_redis_cmd->setnx(key, vl);
		if (re < 0)
		{
			log_error3("(%s)setnx(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
		}
		if (re == 1)
		{
			m_redis_cmd->pexpire(key, 60000);//60������
		}
		return re == 1;
	}
	bool trans_redis::unlock_from_redis(const char* key)
	{
		redis_db_scope scope(REDIS_DB_ZERO_SYSTEM);
		if (m_redis_cmd->del(key) < 0)
		{
			log_error3("(%s)del(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
		}
		return true;
	}

	bool trans_redis::get_hash(const char* key, const char* sub_key, acl::string& vl)
	{
		m_redis_cmd->clear();
		if (!m_redis_cmd->hget(key, sub_key, vl))
		{
			log_error3("(%s)get_hash(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			return false;
		}
		log_debug3(DEBUG_BASE, 5, "redis->hget %s:%s(%s)", key, sub_key, vl.c_str());
		return true;
	}

	bool trans_redis::set_hash(const char* key, const char* sub_key, const char* vl)
	{
		m_redis_cmd->clear();
		acl::string vl2;
		if (m_redis_cmd->hset(key, sub_key, vl) < 0)
		{
			log_error3("(%s)set_hash(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			return false;
		}
		log_debug3(DEBUG_BASE, 5, "redis->hset %s:%s(%s)", key, sub_key, vl);
		return true;
	}
	bool trans_redis::get_hash(const char* key, std::map<acl::string, acl::string>& vl)
	{
		m_redis_cmd->clear();
		if (!m_redis_cmd->hgetall(key, vl))
		{
			log_error3("(%s)get_hash(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			return false;
		}
		log_debug1(DEBUG_BASE, 5, "redis->hget %s", key);
		return true;
	}

	bool trans_redis::del_hash(const char* key, const char* sub_key)
	{
		m_redis_cmd->clear();
		acl::string vl2;
		if (m_redis_cmd->hdel(key, sub_key) < 0)
		{
			log_error3("(%s)set_hash(%s)ʱ��������(%s)", RedisIp, key, m_redis_cmd->result_error());
			return false;
		}
		log_debug2(DEBUG_BASE, 5, "redis->hdel %s:%s", key, sub_key);
		return true;
	}

}
