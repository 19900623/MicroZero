
#ifndef CLIENT
#ifndef CACHE_REDIS_H
#define CACHE_REDIS_H
#pragma once
#include <stdinc.h>
using namespace std;
namespace agebull
{
	//��Ϣ���е���Ϣ����
#define REDIS_DB_ZERO_VOTE 0x15
	//��Ϣ���е���Ϣ����
#define REDIS_DB_ZERO_PLAN 0x14
	//��Ϣ���е���Ϣ����
#define REDIS_DB_ZERO_MESSAGE 0x13
	//վ������
#define REDIS_DB_ZERO_STATION 0x12
	//ϵͳ
#define REDIS_DB_ZERO_SYSTEM 0x11

	class redis_db_scope;
	class redis_trans_scope;
	/**
	* \brief ����Redis ��û������ʱ,����ͨʹ��һ��,����ʱ����begin_trans,�ύ����commit,���˵���rollback,�ұ���ɶԵ���
	*/
	class trans_redis
	{
		friend class redis_db_scope;
		friend class redis_trans_scope;

		/**
		* \brief ��������Ĵ���
		*/
		int m_trans_num;
		/**
		* \brief �Ƿ��л��˵ĵ���,��������м�ʧ��,������ʽ����Ϊ������
		*/
		bool m_failed;
		/**
		* \brief ���һ�β����Ƿ�ɹ�
		*/
		bool m_last_status;
		/**
		* \brief acl��redis�ͻ��˶���
		*/
		acl::redis_client* m_redis_client;
		/**
		* \brief acl��redis�������
		*/
		acl::redis* m_redis_cmd;
		/**
		* \brief �������޸ĵ�����
		*/
		map<acl::string, int> m_modifies;
		/**
		* \brief ���ػ������
		*/
		map<acl::string, acl::string> m_local_values;
	public:
		/**
		* \brief ����
		*/
		trans_redis();
		/**
		* \brief ����
		*/
		~trans_redis();
		/**
		* \brief ȡ�õ�ǰ�߳������ĵ�����Redis����
		* @return ��ǰ�߳������ĵĲ�������
		*/
		static trans_redis& get_context();
		/**
		* \brief ȡ�õ�ǰ�߳������ĵ�����Redis����
		* @return ��ǰ�߳������ĵĲ�������
		*/
		static trans_redis* get_current();
		/**
		* \brief ���ɵ�ǰ�߳������ĵ�����Redis����
		*/
		static bool open_context();
		/**
		* \brief ���ɵ�ǰ�߳������ĵ�����Redis����
		*/
		static	bool open_context(int db);
		/**
		* \brief �رյ�ǰ�߳������ĵ�����Redis����
		*/
		static void close_context();
		/**
		* \brief ��������
		* @return ��ǰ�߳������ĵĲ�������
		*/
		static trans_redis& begin_trans();
		/**
		* \brief �ύ����,�������������������ĵط�����,ֻ�Ǽ����������ô���,���һ�ε���(��Ӧ�������begin_trans)ʱ,���֮ǰm_failed������,�ڲ����ǻ����rollback,����ignore_failed����Ϊtrue
		* @param {bool} ignore_failed ����m_failed������,�����Եĵ����ύ
		*/
		static void end_trans(bool ignore_failed = false);
		/**
		* \brief ���ó���
		*/
		static void set_failed();
		/**
		* \brief ���һ�β����Ƿ�ɹ�
		*/
		bool last_status() const;
	private:
		/**
		* \brief �ύ����
		*/
		void commit_inner();
	public:
		bool get(const char*, acl::string&);
		void set(const char*, const char*);
		void set(const char*, const char*, size_t);
		void set(const char*, acl::string&);
		acl::redis* operator->() const;
		acl::string read_str_from_redis(const char* key);
		template<class TArg1>
		acl::string read_str_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		acl::string read_str_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		acl::string read_str_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		std::vector<acl::string> find_redis_keys(const char* find_key) const;
		template<class TArg1>
		std::vector<acl::string> find_redis_keys(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		std::vector<acl::string> find_redis_keys(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		std::vector<acl::string> find_redis_keys(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		std::vector<acl::string*> find_from_redis(const char* find_key);
		template<class TArg1>
		std::vector<acl::string*> find_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		std::vector<acl::string*> find_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		std::vector<acl::string*> find_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);
		acl::string read_from_redis(const char* key);
		template<class TArg1>
		acl::string read_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		acl::string read_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		acl::string read_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		void delete_from_redis(const char* find_key) const;

		template<class TArg1>
		void delete_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		void delete_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		void delete_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		size_t incr_redis(const char* key) const;
		template<class TArg1>
		size_t incr_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		size_t incr_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		size_t incr_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);
		void write_to_redis(const char* key, const char* bin, size_t len);
		void write_json_to_redis(const char* key, const char* json);
		template<class TArg1>
		void write_to_redis(const char* bin, size_t len, const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		void write_to_redis(const char* bin, size_t len, const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		void write_to_redis(const char* bin, size_t len, const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);


		acl::string read_first_from_redis(const char* key);
		template<class TArg1>
		acl::string read_first_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		acl::string read_first_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		acl::string read_first_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);


		bool unlock_from_redis(const char* key) const;
		template<class TArg1>
		bool unlock_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		bool unlock_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		bool unlock_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		bool lock_from_redis(const char* key) const;
		template<class TArg1>
		bool lock_from_redis(const char* key_fmt, TArg1 arg);

		template<class TArg1, class TArg2>
		bool lock_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2);

		template<class TArg1, class TArg2, class TArg3>
		bool lock_from_redis(const char* key_fmt, TArg1 arg1, TArg2 arg2, TArg3 arg3);

		bool get_hash(const char* key, const char* sub_key, acl::string& vl) const;
		bool set_hash(const char* key, const char* sub_key, const char* vl) const;
		bool del_hash(const char* key, const char* sub_key) const;
		bool get_hash(const char* key, std::map<acl::string, acl::string>& vl) const;
	};
	/**
	* \brief Redis��ǰ�����Ķ������淶Χ
	*/
	class redis_live_scope
	{
		bool open_by_me_;
	public:
		/**
		* \brief ����
		*/
		redis_live_scope();

		/**
		* \brief ����
		*/
		redis_live_scope(int db);

		/**
		* \brief ����
		*/
		~redis_live_scope();
	};
	/**
	* \brief �Զ��ָ�������ID��Χ
	*/
	class redis_db_scope
	{
		trans_redis& redis_;
	public:
		/**
		* \brief ����
		*/
		redis_db_scope(int db);
		/**
		* \brief ����
		*/
		~redis_db_scope();
	};
	/**
	* \brief �Զ��������ύ������Χ
	*/
	class redis_trans_scope
	{
	public:
		/**
		* \brief ����
		*/
		redis_trans_scope();

		/**
		* \brief ����
		*/
		~redis_trans_scope();
	};
}
#endif
#endif
