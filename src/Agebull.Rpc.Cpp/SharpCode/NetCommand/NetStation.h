#pragma once
#ifndef C_ZMQ_NET_OBJECT
#include <stdinc.h>
#include <zeromq/zhelpers.h>

#define STATION_TYPE_DISPATCHER 1
#define STATION_TYPE_API 2
#define STATION_TYPE_VOTE 3
#define STATION_TYPE_PUBLISH 4

namespace agebull
{
	namespace zmq_net
	{
		class NetStation;
		/**
		* @brief ����վ��ʵ������վ��ֿ⣬�ǲ��Ǻ��Զ������֣�
		*/
		class StationWarehouse
		{
		protected:
			/**
			* @brief ʵ������
			*/
			static map<string, NetStation*> examples;
		public:
			/**
			* @brief ������з���
			*/
			static void clear();
			/**
			* @brief ��ԭ����
			*/
			static int restore();
			/**
			* @brief ��ʼ������
			*/
			static acl::string install(int station_type, const char* station_name);
			/**
			* @brief ��ԭ����
			*/
			static bool restore(acl::string& value);
			/**
			* @brief �������
			*/
			static bool join(NetStation* station);
			/**
			* @brief �������
			*/
			static bool left(NetStation* station);
			/**
			* @brief �������
			*/
			static NetStation* find(string name);
		};
		/**
		 * \brief  վ��״̬
		 */
		enum class station_state
		{
			/**
			 * \brief �ޣ��չ���
			 */
			None,
			/**
			* \brief ��������
			*/
			Start,
			/**
			* \brief ��������
			*/
			Run,
			/**
			* \brief ����ͣ
			*/
			Pause,
			/**
			* \brief ��Ҫ�ر�
			*/
			Closing,
			/**
			 * \brief �ѹر�
			 */
			Closed,
			/**
			 * \brief �����٣������ѵ���
			 */
			Destroy
		};

		/**
		* @brief ��ʾһ������ZMQ������վ��
		*/
		class NetStation
		{
			friend StationWarehouse;
		protected:
			/**
			* @brief ״̬�ź���
			*/
			boost::interprocess::interprocess_semaphore _state_semaphore;
		protected:
			/**
			* @brief API��������
			*/
			string _station_name;

			/**
			* @brief �ⲿ��ַ
			*/
			string _out_address;

			/**
			* @brief ������ַ
			*/
			string _inner_address;

			/**
			* @brief ������ַ
			*/
			string _heart_address;
			/**
			* @brief ���þ��
			*/
			void* _out_socket;

			/**
			* @brief �������
			*/
			void* _inner_socket;
			/**
			* @brief �������
			*/
			void* _heart_socket;

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

			/**
			* @brief ��ǰZMQִ��״̬
			*/
			int _zmq_state;

		public:
			/**
			* @brief API��������
			*/
			int _station_type;
			/**
			* @brief ��ǰվ��״̬
			*/
			station_state _station_state;

			/**
			* @brief ����
			*/
			NetStation(string name, int type, int out_zmq_type, int inner_zmq_type, int heart_zmq_type);

			/**
			* @brief ����
			*/
			virtual ~NetStation()
			{
				close(true);
				_station_state = station_state::Destroy;
			}

		protected:
			/**
			* @brief ����ѭ��
			*/
			bool poll();
			/**
			* @brief ������
			*/
			virtual void response() = 0;
			/**
			* @brief ��������
			*/
			virtual void request() = 0;

			/**
			* ��������Ӧ
			*/
			virtual void heartbeat() =0;
		protected:
			/**
			 * @brief ���տ�֡
			 */
			static void recv_empty(ZMQ_HANDLE socket);


		protected:
			/**
			* @brief �ܷ��������
			*/
			bool can_do() const
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) &&
					get_net_state() == NET_STATE_RUNING;
			}
			/**
			* @brief ����Ƿ���ͣ
			*/
			bool check_pause()
			{
				if (_station_state == station_state::Pause)
				{
					boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());
					_state_semaphore.timed_wait(now + boost::posix_time::seconds(1));
					return _station_state == station_state::Pause;
				}
				return false;
			}
		public:

			/**
			* @brief ��ͣ
			*/
			bool pause(bool waiting)
			{
				if (station_state::Run != _station_state)
					return false;
				_station_state = station_state::Pause;
				return true;
			}

			/**
			* @brief ����
			*/
			bool resume(bool waiting)
			{
				if (station_state::Pause != _station_state)
					return false;
				_station_state = station_state::Run;
				_state_semaphore.post();
				return true;
			}

			/**
			* @brief ����
			*/
			bool close(bool waiting)
			{
				if (!can_do())
					return false;
				_station_state = station_state::Closing;
				while (waiting && _station_state == station_state::Closing)
					thread_sleep(1000);
				return true;
			}
			/**
			* @brief ��ʼִ��һ������
			*/
			virtual void command_start(const char* caller, vector< string> lines) = 0;
			/**
			* @brief ����ִ��һ������
			*/
			virtual void command_end(const char* caller, vector< string> lines) = 0;
		};

		/**
		 * \brief ��ʾһ������ZMQ�ĸ��ؾ���վ��
		 * \tparam TNetStation
		 * \tparam TWorker
		 * \tparam NetType
		 */
		template <typename TNetStation, class TWorker, int NetType>
		class BalanceStation : public NetStation
		{
		protected:
			/**
			* @brief �����߼���
			*/
			map<string, TWorker> _workers;
		public:
			/**
			* @brief ����
			*/
			BalanceStation(string name)
				: NetStation(name, NetType, ZMQ_ROUTER, ZMQ_ROUTER, ZMQ_REQ)
			{
			}

		protected:

			/**
			* @brief ���ɹ�������
			*/
			virtual TWorker create_item(const char* addr, const char* value) = 0;

			/**
			* ��������Ӧ
			*/
			virtual void heartbeat() override;
			/**
			* @brief ���������˳�
			*/
			virtual void worker_left(char* addr) ;

			/**
			* @brief �����������
			*/
			virtual void worker_join(char* addr, char* value, bool ready = false);
		};




#define close_socket(socket,addr)\
		zmq_unbind(socket, addr.c_str());\
		zmq_close(socket);\
		socket = nullptr
		/**
		 * \brief
		 * \param name
		 * \param type
		 */
		inline NetStation::NetStation(string name, int type, int out_zmq_type, int inner_zmq_type, int heart_zmq_type)
			: _state_semaphore(1)
			, _station_name(name)
			, _station_type(type)
			, _out_socket(nullptr)
			, _inner_socket(nullptr)
			, _heart_socket(nullptr)
			, _out_zmq_type(out_zmq_type)
			, _inner_zmq_type(inner_zmq_type)
			, _heart_zmq_type(heart_zmq_type)
			, _station_state(station_state::None)
			, _zmq_state(0)
		{
		}

		/**
		 * \brief
		 * \param socket
		 */
		inline void NetStation::recv_empty(ZMQ_HANDLE socket)
		{
			char* empty = s_recv(socket, 0);
			if (empty != nullptr)
			{
				assert(empty[0] == 0);
				free(empty);
			}
		}

		/**
		 * \brief
		 * \return
		 */
		inline bool NetStation::poll()
		{
			_zmq_state = 0;
			int cnt = 0;
			zmq_pollitem_t items[3];
			if (_out_zmq_type >= 0)
			{
				_out_socket = create_socket(_out_zmq_type, _out_address.c_str());
				items[cnt++] = { _out_socket, 0, ZMQ_POLLIN, 0 };
			}
			if (_inner_zmq_type >= 0)
			{
				_inner_socket = create_socket(_inner_zmq_type, _inner_address.c_str());
				items[cnt++] = { _inner_socket, 0, ZMQ_POLLIN, 0 };
			}
			if (_heart_zmq_type >= 0)
			{
				_heart_socket = create_socket(_heart_zmq_type, _heart_address.c_str());
				items[cnt++] = { _heart_socket, 0, ZMQ_POLLIN, 0 };
			}

			log_msg3("%s(%s | %s)������", _station_name, _out_address, _inner_address);
			//�Ǽ��߳̿�ʼ
			set_command_thread_start();
			_station_state = station_state::Run;
			while (can_do())
			{
				if (check_pause())
					continue;

				_zmq_state = zmq_poll(items, cnt, 1000);
				if (_zmq_state == 0)
					continue;
				if (_zmq_state == -1)
					break;
				// ����_inner_socket��inner�Ķ���
				if (items[0].revents & ZMQ_POLLIN)
				{
					response();
				}

				if (cnt > 1 && items[1].revents & ZMQ_POLLIN)
				{
					request();
				}

				if (cnt > 2 && items[2].revents & ZMQ_POLLIN)
				{
					heartbeat();
				}
			}
			_station_state = station_state::Closing;
			if (_out_zmq_type >= 0)
			{
				close_socket(_out_socket, _out_address);
			}
			if (_inner_zmq_type >= 0)
			{
				close_socket(_inner_socket, _inner_address);
			}
			if (_heart_zmq_type >= 0)
			{
				close_socket(_heart_socket, _heart_address);
			}
			//�Ǽ��̹߳ر�
			set_command_thread_end();
			_station_state = station_state::Closed;
			return _zmq_state < 0;
		}

		/**
		 * \brief
		 */
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::heartbeat()
		{
			// ��inner�ĵ�ַ���
			char* inner_addr = s_recv(_heart_socket);
			recv_empty(_heart_socket);
			char* client_addr = s_recv(_heart_socket);
			recv_empty(_heart_socket);
			char* reply = s_recv(_heart_socket);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(client_addr, "PAPA") == 0)
			{
				worker_join(inner_addr, reply);
			}
			else if (strcmp(client_addr, "MAMA") == 0)
			{
				worker_join(inner_addr, reply);
			}
			else if (strcmp(client_addr, "LAOWANG") == 0)
			{
				worker_left(inner_addr);
			}
			_zmq_state = s_sendmore(_heart_socket, inner_addr);
			if (_zmq_state < 0)
				cout << inner_addr << endl;
			_zmq_state = s_sendmore(_heart_socket, "");
			_zmq_state = s_send(_heart_socket, "OK");//��ʵ����
			if (_zmq_state <= 0)
				cout << inner_addr << endl;

			free(inner_addr);
			free(client_addr);
			free(reply);
		}

		/**
		 * \brief
		 * \param addr
		 */
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::worker_left(char* addr)
		{
			auto vote = _workers.find(addr);
			if (vote != _workers.end())
			{
				_workers.erase(addr);
				cout << addr << "���˳�" << endl;
			}
		}

		/**
		 * \brief
		 * \param addr
		 * \param value
		 * \param ready
		 */
		template <typename TNetStation, class TWorker, int NetType>
		void BalanceStation<TNetStation, TWorker, NetType>::worker_join(char* addr, char* value, bool ready)
		{
			TWorker item = create_item(addr, value);
			auto old = _workers.find(addr);
			if (old == _workers.end())
			{
				_workers.insert(make_pair(addr, item));

				cout << addr << "(" << addr << ")�Ѽ���(ͨ��" << (ready ? "����)" : "����)") << endl;
			}
			else
			{
				old->second = item;
				cout << addr << "(" << addr << ")������(ͨ������)" << endl;
			}
		}


		//��ʼ�̵߳ĺ�
#define station_run(station)\
			if(!StationWarehouse::join(station.get()))\
				return;\
			if (station->_zmq_state == 0)\
				log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);\
			else\
				log_msg3("%s(%s | %s)��������", station->_station_name, station->_out_address, station->_inner_address);\
			bool reStrart = station->poll();\
			StationWarehouse::left(station.get());\
			if (reStrart)\
			{\
				run(station->_station_name);\
			}\
			else\
			{\
				log_msg3("%s(%s | %s)�ѹر�", station->_station_name, station->_out_address, station->_inner_address);\
			}


	}
}



#endif
