#pragma once
#ifndef C_ZMQ_NET_OBJECT
#include <stdinc.h>
#include "sharp_char.h"
#include "zmq_extend.h"
#include "StationWarehouse.h"



namespace agebull
{
	namespace zmq_net
	{
#define STATION_TYPE_DISPATCHER 1
#define STATION_TYPE_MONITOR 2
#define STATION_TYPE_API 3
#define STATION_TYPE_VOTE 4
#define STATION_TYPE_PUBLISH 5

		/**
		* @brief ��ʾһ������ZMQ������վ��
		*/
		class ZeroStation
		{
			friend class StationWarehouse;
		protected:
			bool _in_plan_poll;

			acl::string _config;
			/**
			* @brief ״̬�ź���
			*/
			boost::interprocess::interprocess_semaphore _state_semaphore;
			/**
			* @brief API��������
			*/
			string _station_name;

			/**
			* @brief վ������
			*/
			int _station_type;

			/**
			* @brief �ⲿ��ַ
			*/
			int _out_port;

			/**
			* @brief ������ַ
			*/
			int _inner_port;

			/**
			* @brief ������ַ
			*/
			int _heart_port;
			/**
			* @brief �ⲿSOCKET����
			*/
			int _out_zmq_type;

			/**
			* @brief ����SOCKET����
			*/
			int _inner_zmq_type;

			/**
			* @brief ����SOCKET����
			*/
			int _heart_zmq_type;
		protected:
			/*
			*@brief ��ѯ�ڵ�
			*/
			zmq_pollitem_t* _poll_items;
			/*
			*@brief �ڵ�����
			*/
			int _poll_count;
			/**
			* @brief ���þ��
			*/
			void* _out_socket;
			/**
			* @brief ���þ��
			*/
			void* _out_socket_inproc;
			/**
			* @brief �������
			*/
			void* _inner_socket;
			/**
			* @brief �������
			*/
			void* _heart_socket;
			/**
			* @brief ��ǰZMQִ��״̬
			*/
			ZmqSocketState _zmq_state;
			/**
			* @brief ��ǰվ��״̬
			*/
			station_state _station_state;
		public:

			const char* get_config() const
			{
				return _config.c_str();
			}
			/**
			* @brief ��ǰZMQִ��״̬
			*/
			ZmqSocketState get_zmq_state() const
			{
				return _zmq_state;
			}
			/**
			* @brief API��������
			*/
			int get_station_type() const
			{
				return _station_type;
			}
			/**
			* @brief ��ǰվ��״̬
			*/
			station_state get_station_state() const
			{
				return _station_state;
			}
			/**
			* @brief API��������
			*/
			const string& get_station_name() const
			{
				return _station_name;
			}

			/**
			* @brief �ⲿ��ַ
			*/
			string get_out_address()const
			{
				string addr("tcp://*:");
				addr += _out_port;
				return addr;
			}

			/**
			* @brief ������ַ
			*/
			string get_inner_address()const
			{
				string addr("tcp://*:");
				addr += _inner_port;
				return addr;
			}

			/**
			* @brief ������ַ
			*/
			string get_heart_address()const
			{
				string addr("tcp://*:");
				addr += _heart_port;
				return addr;
			}
			/**
			* @brief ����
			*/
			ZeroStation(string name, int type, int out_zmq_type, int inner_zmq_type, int heart_zmq_type)
				: _state_semaphore(1)
				, _station_name(name)
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
				, _in_plan_poll(false)
			{
			}

		public:
			/**
			* @brief ����
			*/
			virtual ~ZeroStation()
			{
				ZeroStation::close(true);
				_station_state = station_state::Destroy;
			}
			/**
			* @brief �ܷ��������
			*/
			virtual bool can_do() const
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) && get_net_state() == NET_STATE_RUNING;
			}
			/**
			* @brief ����Ƿ���ͣ
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
		public:
			/**
			* @brief ��ʼ��
			*/
			bool initialize();

			/**
			* @brief ������ѯ
			*/
			virtual bool poll();

			/**
			* @brief ������
			*/
			virtual void response() {}
			/**
			* @brief ��������
			*/
			virtual void request(ZMQ_HANDLE socket) {}
			/**
			* ��������Ӧ
			*/
			virtual void heartbeat() {}

			/**
			* @brief ����
			*/
			bool destruct();

			/**
			* @brief ��ͣ
			*/
			virtual	bool pause(bool waiting);
			/**
			* @brief ����
			*/
			virtual bool resume(bool waiting);
			/**
			* @brief ����
			*/
			virtual	bool close(bool waiting);
			/**
			* @brief ִ��һ������
			*/
			virtual sharp_char command(const char* caller, vector<string> lines) = 0;
		public:
			/**
			* @brief �ƻ���ѯ
			*/
			static void plan_poll(void* arg)
			{
				ZeroStation* station = static_cast<ZeroStation*>(arg);
				station->plan_poll_();
			}
		protected:
			/**
			* @brief �ƻ���ѯ
			*/
			void plan_poll_();

			/**
			* \brief �������ڵ��ڵ�����
			*/
			int load_now(vector<PlanMessage>& messages) const;

			/**
			* \brief ɾ��һ���ƻ�
			*/
			bool remove(PlanMessage& message) const;

			/**
			* \brief �ƻ���һ��ִ��ʱ��
			*/
			bool plan_next(PlanMessage& message, bool first) const;

			/**
			* \brief ������һ��ִ��ʱ��
			*/
			bool save_next(PlanMessage& message) const;

			/**
			* \brief ��������
			*/
			static bool save_message(PlanMessage& message);

			/**
			* \brief ��ȡ����
			*/
			static bool load_message(const char* key, PlanMessage& message);
		};


		/**
		* @brief ��ʼ��
		*/
		inline bool ZeroStation::initialize()
		{
			_station_state = station_state::Start;
			_zmq_state = ZmqSocketState::Succeed;
			_poll_count = 0;

			if (_out_zmq_type >= 0 && _out_zmq_type != ZMQ_PUB)
			{
				_poll_count += 2;
			}
			if (_inner_zmq_type >= 0 && _inner_zmq_type != ZMQ_PUB)
			{
				_poll_count++;
			}
			if (_heart_zmq_type >= 0 && _heart_zmq_type != ZMQ_PUB)
			{
				_poll_count++;
			}
			_poll_items = new zmq_pollitem_t[_poll_count];
			memset(_poll_items, 0, sizeof(zmq_pollitem_t) * _poll_count);
			int cnt = 0;

			if (_out_zmq_type >= 0)
			{
				_out_socket = create_server_socket(_station_name.c_str(), _out_zmq_type, _out_port);
				if (_out_socket == nullptr)
				{
					_station_state = station_state::Failed;
					log_error2("%s initialize error(out) %s", _station_name, zmq_strerror(zmq_errno()));
					set_command_thread_bad(_station_name.c_str());
					return false;
				}
				_out_socket_inproc = create_server_socket(_station_name.c_str(), _out_zmq_type);
				if (_out_socket_inproc == nullptr)
				{
					_station_state = station_state::Failed;
					log_error2("%s initialize error(inproc) %s", _station_name, zmq_strerror(zmq_errno()));
					set_command_thread_bad(_station_name.c_str());
					return false;
				}
				if (_out_zmq_type != ZMQ_PUB)
				{
					_poll_items[cnt++] = { _out_socket, 0, ZMQ_POLLIN, 0 };
					_poll_items[cnt++] = { _out_socket_inproc, 0, ZMQ_POLLIN, 0 };
				}
			}
			if (_inner_zmq_type >= 0)
			{
				_inner_socket = create_server_socket(_station_name.c_str(), _inner_zmq_type, _inner_port);
				if (_inner_socket == nullptr)
				{
					_station_state = station_state::Failed;
					log_error2("%s initialize error(inner) %s", _station_name, zmq_strerror(zmq_errno()));
					set_command_thread_bad(_station_name.c_str());
					return false;
				}
				if (_inner_zmq_type != ZMQ_PUB)
					_poll_items[cnt++] = { _inner_socket, 0, ZMQ_POLLIN, 0 };
			}
			if (_heart_zmq_type >= 0)
			{
				_heart_socket = create_server_socket(_station_name.c_str(), _heart_zmq_type, _heart_port);
				if (_heart_socket == nullptr)
				{
					_station_state = station_state::Failed;
					log_error2("%s initialize error(heart) %s", _station_name, zmq_strerror(zmq_errno()));
					set_command_thread_bad(_station_name.c_str());
					return false;
				}
				if (_heart_zmq_type != ZMQ_PUB)
					_poll_items[cnt] = { _heart_socket, 0, ZMQ_POLLIN, 0 };
			}
			_station_state = station_state::Run;
			return true;
		}

		/**
		* @brief ����
		*/
		inline bool ZeroStation::destruct()
		{
			if (_poll_items == nullptr)
				return true;
			delete[]_poll_items;
			_poll_items = nullptr;
			_station_state = station_state::Closing;
			if (_out_socket != nullptr)
			{
				close_socket(_out_socket, get_out_address());
			}
			if (_inner_socket != nullptr)
			{
				close_socket(_inner_socket, get_inner_address());
			}
			if (_heart_socket != nullptr)
			{
				close_socket(_heart_socket, get_heart_address());
			}
			//�Ǽ��̹߳ر�
			set_command_thread_end(_station_name.c_str());
			_station_state = station_state::Closed;
			return true;
		}
		/**
		 * \brief
		 * \return
		 */
		inline bool ZeroStation::poll()
		{
			set_command_thread_start(_station_name.c_str());
			//�Ǽ��߳̿�ʼ
			while (true)
			{
				if (!can_do())
				{
					_zmq_state = ZmqSocketState::Intr;
					break;
				}
				if (check_pause())
					continue;
				if (!can_do())
				{
					_zmq_state = ZmqSocketState::Intr;
					break;
				}

				int state = zmq_poll(_poll_items, _poll_count, 1000);
				if (state == 0)//��ʱ
					continue;
				if (_station_state == station_state::Pause)
					continue;
				if (state < 0)
				{
					_zmq_state = check_zmq_error();
					check_zmq_state()
				}
				for (int idx = 0; idx < _poll_count; idx++)
				{
					if (_poll_items[idx].revents & ZMQ_POLLIN)
					{
						if (_poll_items[idx].socket == _out_socket || _poll_items[idx].socket == _out_socket_inproc)
							request(_poll_items[idx].socket);
						else if (_poll_items[idx].socket == _inner_socket)
							response();
						else if (_poll_items[idx].socket == _heart_socket)
							heartbeat();
						check_zmq_state()
					}
					else if (_poll_items[idx].revents & ZMQ_POLLERR)
					{
						cout << "error";
						//ERROR
					}
				}
			}
			return _zmq_state < ZmqSocketState::Term && _zmq_state > ZmqSocketState::Empty;
		}

	}
}



#endif
