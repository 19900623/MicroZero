#pragma once
#ifndef C_ZMQ_NET_OBJECT
#include <stdinc.h>
 #include <utility>
#include "sharp_char.h"
#include "zmq_extend.h"
#include "StationWarehouse.h"
namespace agebull
{
	namespace zmq_net
	{
		/**
		* \brief ��ʾһ������ZMQ������վ��
		*/
		class ZeroStation
		{
			friend class StationWarehouse;
		protected:
			bool _in_plan_poll;

			acl::string _config;
			/**
			* \brief ״̬�ź���
			*/
			boost::interprocess::interprocess_semaphore _state_semaphore;
			/**
			* \brief API��������
			*/
			string _station_name;

			/**
			* \brief վ������
			*/
			int _station_type;

			/**
			* \brief �ⲿ��ַ
			*/
			int _out_port;

			/**
			* \brief ������ַ
			*/
			int _inner_port;

			/**
			* \brief ������ַ
			*/
			int _heart_port;
			/**
			* \brief �ⲿSOCKET����
			*/
			int _out_zmq_type;

			/**
			* \brief ����SOCKET����
			*/
			int _inner_zmq_type;

			/**
			* \brief ����SOCKET����
			*/
			int _heart_zmq_type;
			/*
			*\brief ��ѯ�ڵ�
			*/
			zmq_pollitem_t* _poll_items;
			/*
			*\brief �ڵ�����
			*/
			int _poll_count;
			/**
			* \brief ���þ��
			*/
			void* _out_socket;
			/**
			* \brief ���þ��
			*/
			void* _out_socket_inproc;
			/**
			* \brief �������
			*/
			void* _inner_socket;
			/**
			* \brief �������
			*/
			void* _heart_socket;
			/**
			* \brief ��ǰZMQִ��״̬
			*/
			ZmqSocketState _zmq_state;
			/**
			* \brief ��ǰվ��״̬
			*/
			station_state _station_state;
		public:
			const char* get_config() const
			{
				return _config.c_str();
			}
			/**
			* \brief ��ǰZMQִ��״̬
			*/
			ZmqSocketState get_zmq_state() const
			{
				return _zmq_state;
			}
			/**
			* \brief API��������
			*/
			int get_station_type() const
			{
				return _station_type;
			}
			/**
			* \brief ��ǰվ��״̬
			*/
			station_state get_station_state() const
			{
				return _station_state;
			}
			/**
			* \brief API��������
			*/
			const string& get_station_name() const
			{
				return _station_name;
			}

			/**
			* \brief �ⲿ��ַ
			*/
			string get_out_address()const
			{
				string addr("tcp://*:");
				addr += _out_port;
				return addr;
			}

			/**
			* \brief ������ַ
			*/
			string get_inner_address()const
			{
				string addr("tcp://*:");
				addr += _inner_port;
				return addr;
			}

			/**
			* \brief ������ַ
			*/
			string get_heart_address()const
			{
				string addr("tcp://*:");
				addr += _heart_port;
				return addr;
			}

			/**
			* \brief ����
			*/
			ZeroStation(string name, int type, int out_zmq_type, int inner_zmq_type, int heart_zmq_type)
				: _in_plan_poll(false)
				, _state_semaphore(1)
				, _station_name(std::move(name))
				, _station_type(type)
				, _out_port(0)
				, _inner_port(0)
				, _heart_port(0)
				, _out_zmq_type(out_zmq_type)
				, _inner_zmq_type(inner_zmq_type)
				, _heart_zmq_type(heart_zmq_type)
				, _poll_items(nullptr)
				, _poll_count(0)
				, _out_socket(nullptr)
				, _out_socket_inproc(nullptr)
				, _inner_socket(nullptr)
				, _heart_socket(nullptr)
				, _zmq_state(ZmqSocketState::Succeed)
				, _station_state(station_state::None)
			{
			}
			/**
			* \brief �������ڵ��ڵ�����
			*/
			size_t load_now(vector<Message>& messages) const;

			/**
			* \brief ɾ��һ���ƻ�
			*/
			bool remove_next(Message& message) const;

			/**
			* \brief �ƻ���һ��ִ��ʱ��
			*/
			bool plan_next(Message& message, bool first) const;

			/**
			* \brief ������һ��ִ��ʱ��
			*/
			bool save_next(Message& message) const;

			/**
			* \brief ������Ϣ
			*/
			bool save_message(Message& message) const;

			/**
			* \brief ��ȡ��Ϣ
			*/
			bool load_message(uint id, Message& message) const;

			/**
			* \brief ɾ����Ϣ
			*/
			bool remove_message(Message& message) const;

			/**
			* \brief ������Ϣ������
			*/
			bool save_message_worker(uint msgid, vector<const char*>& workers) const;

			/**
			* \brief ������Ϣ�����߷���ֵ
			*/
			bool save_message_result(uint msgid, const string& worker, const string& response) const;

			/**
			* \brief ȡһ�������ߵ���Ϣ����ֵ
			*/
			acl::string get_message_result(uint msgid, const char* worker) const;

			/**
			* \brief ȡȫ����������Ϣ����ֵ
			*/
			map<acl::string, acl::string> get_message_result(uint msgid) const;

			/**
			* \brief ִ��һ������
			*/
			virtual sharp_char command(const char* caller, vector<sharp_char> lines) = 0;
		public:

			/**
			*\brief ������Ϣ
			*/
			bool send_data(vector<sharp_char>& datas)
			{
				_zmq_state = send(_inner_socket, datas);
				return _zmq_state == ZmqSocketState::Succeed;
			}
			/**
			* \brief ����
			*/
			virtual ~ZeroStation()
			{
				ZeroStation::close(true);
				_station_state = station_state::Destroy;
			}
			/**
			* \brief �ܷ��������
			*/
			virtual bool can_do() const
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) && get_net_state() == NET_STATE_RUNING;
			}
			/**
			* \brief ����Ƿ���ͣ
			*/
			bool check_pause()
			{
				if (_station_state == station_state::Pause)
				{
					_state_semaphore.timed_wait(time_span(1000));
					return _station_state == station_state::Pause;
				}
				return false;
			}
			/**
			* \brief ��ʼ��
			*/
			bool initialize();

			/**
			* \brief ������ѯ
			*/
			bool poll();

			/**
			* \brief ����
			*/
			bool destruct();

			/**
			* \brief ��ͣ
			*/
			virtual bool pause(bool waiting);
			/**
			* \brief ����
			*/
			virtual bool resume(bool waiting);
			/**
			* \brief ����
			*/
			virtual bool close(bool waiting);
			/**
			* \brief �ƻ���ѯ
			*/
			static void plan_poll(void* arg)
			{
				ZeroStation* station = static_cast<ZeroStation*>(arg);
				station->plan_poll_();
			}
		protected:
			/**
			* \brief �ƻ���ѯ
			*/
			void plan_poll_();

			/**
			* \brief �������ϵ���Ӧ
			*/
			virtual void response() {}
			/**
			* \brief ���ü��ϵ���Ӧ
			*/
			virtual void request(ZMQ_HANDLE socket) = 0;
			/**
			* ��������Ӧ
			*/
			virtual void heartbeat(){}
		};


	}
}
#endif
