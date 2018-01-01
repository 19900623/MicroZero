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
			* @brief API��������
			*/
			int _station_type;

			/**
			* @brief �ⲿ��ַ
			*/
			string _outAddress;

			/**
			* @brief ������ַ
			*/
			string _innerAddress;

			/**
			* @brief ������ַ
			*/
			string _heartAddress;
			/**
			* @brief ���þ��
			*/
			void* _outSocket;

			/**
			* @brief �������
			*/
			void* _innerSocket;
			/**
			* @brief �������
			*/
			void* _heartSocket;

			/**
			* @brief ��ǰվ��״̬
			*/
			station_state _station_state;

			/**
			* @brief ��ǰZMQִ��״̬
			*/
			int _zmq_state;
		public:
			/**
			* @brief ����
			*/
			NetStation(string name, int type);

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
			 * @brief ���տ�֡
			 */
			static void recv_empty(ZMQ_HANDLE socket);

			/**
			* @brief �ܷ��������
			*/
			bool can_do() const
			{
				return (_station_state == station_state::Run || _station_state == station_state::Pause) &&
					get_net_state() == NET_STATE_RUNING;
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
		};

		/**
		 * \brief ��ʾһ������ZMQ��·��վ��
		 * \tparam TNetStation 
		 * \tparam TWorker 
		 * \tparam NetType 
		 */
		template <typename TNetStation, class TWorker, int NetType>
		class RouteStation : public NetStation
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
			RouteStation(string name)
				: NetStation(name, NetType)
			{
			}

		protected:
			/**
			* @brief ִ��
			*/
			bool poll();

			/**
			* @brief �������ϵ���Ӧ
			*/
			virtual void onWorkerPollIn() = 0;
			/**
			* @brief ���ü��ϵ���Ӧ
			*/
			virtual void onCallerPollIn() = 0;

			/**
			* ��������Ӧ
			*/
			virtual void heartbeat();

			/**
			* @brief ���ɹ�������
			*/
			virtual TWorker create_item(const char* addr, const char* value) = 0;

			/**
			* @brief ���������˳�
			*/
			virtual void left(char* addr);

			/**
			* @brief �����������
			*/
			virtual void join(char* addr, char* value, bool ready = false);
		};

#define poll_check_pause()\
	if (_station_state == station_state::Pause)\
	{\
		do\
		{\
			boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());\
			_state_semaphore.timed_wait(now + boost::posix_time::seconds(1));\
		} while (_station_state == station_state::Pause && get_net_state() == NET_STATE_RUNING);\
		if (!can_do())\
			break;\
	}

#define poll_zmq_poll(cnt)\
		_zmq_state = zmq_poll(items, cnt, 1000);\
		if (_zmq_state == 0)\
			continue;\
		if (_zmq_state == -1)\
			break;

		/**
		 * \brief 
		 * \param name 
		 * \param type 
		 */
		inline NetStation::NetStation(string name, int type)
			: _state_semaphore(1)
			, _station_name(name)
			, _station_type(type)
			, _outSocket(nullptr)
			, _innerSocket(nullptr)
			, _heartSocket(nullptr)
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
		template <typename TNetStation, class TWorker, int NetType>
		bool RouteStation<TNetStation, TWorker, NetType>::poll()
		{
			_zmq_state = 0;
			_outSocket = create_socket(ZMQ_ROUTER, _outAddress.c_str());
			_innerSocket = create_socket(ZMQ_ROUTER, _innerAddress.c_str());
			_heartSocket = create_socket(ZMQ_ROUTER, _heartAddress.c_str());
			zmq_pollitem_t items[] = {
				{_innerSocket, 0, ZMQ_POLLIN, 0},
				{_outSocket, 0, ZMQ_POLLIN, 0},
				{_heartSocket, 0, ZMQ_POLLIN, 0}
			};
			log_msg3("%s(%s | %s)������", _station_name, _outAddress, _innerAddress);
			//�Ǽ��߳̿�ʼ
			set_command_thread_start();
			_station_state = station_state::Run;
			while (can_do())
			{
				poll_check_pause();
				poll_zmq_poll(3);
				// ����_innerSocket��inner�Ķ���
				if (items[0].revents & ZMQ_POLLIN)
				{
					onWorkerPollIn();
				}

				if (items[1].revents & ZMQ_POLLIN)
				{
					onCallerPollIn();
				}

				if (items[2].revents & ZMQ_POLLIN)
				{
					heartbeat();
				}
			}
			_station_state = station_state::Closing;
			zmq_unbind(_outSocket, _outAddress.c_str());
			zmq_close(_outSocket);
			_outSocket = nullptr;
			zmq_unbind(_innerSocket, _innerAddress.c_str());
			zmq_close(_innerSocket);
			_innerSocket = nullptr;
			zmq_unbind(_heartSocket, _heartAddress.c_str());
			zmq_close(_heartSocket);
			_heartSocket = nullptr;
			//�Ǽ��̹߳ر�
			set_command_thread_end();
			_station_state = station_state::Closed;
			return _zmq_state < 0;
		}

		/**
		 * \brief
		 */
		template <typename TNetStation, class TWorker, int NetType>
		void RouteStation<TNetStation, TWorker, NetType>::heartbeat()
		{
			// ��inner�ĵ�ַ���
			char* inner_addr = s_recv(_heartSocket);
			recv_empty(_heartSocket);
			char* client_addr = s_recv(_heartSocket);
			recv_empty(_heartSocket);
			char* reply = s_recv(_heartSocket);
			// �����һ��Ӧ����Ϣ����ת����client
			if (strcmp(client_addr, "PAPA") == 0)
			{
				join(inner_addr, reply);
			}
			else if (strcmp(client_addr, "MAMA") == 0)
			{
				join(inner_addr, reply);
			}
			else if (strcmp(client_addr, "LAOWANG") == 0)
			{
				left(inner_addr);
			}
			_zmq_state = s_sendmore(_heartSocket, inner_addr);
			if (_zmq_state < 0)
				cout << inner_addr << endl;
			_zmq_state = s_sendmore(_heartSocket, "");
			_zmq_state = s_send(_heartSocket, "OK");//��ʵ����
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
		void RouteStation<TNetStation, TWorker, NetType>::left(char* addr)
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
		void RouteStation<TNetStation, TWorker, NetType>::join(char* addr, char* value, bool ready)
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
				log_msg3("%s(%s | %s)��������", station->_station_name, station->_outAddress, station->_innerAddress);\
			else\
				log_msg3("%s(%s | %s)��������", station->_station_name, station->_outAddress, station->_innerAddress);\
			bool reStrart = station->poll();\
			StationWarehouse::left(station.get());\
			if (reStrart)\
			{\
				run(station->_station_name);\
			}\
			else\
			{\
				log_msg3("%s(%s | %s)�ѹر�", station->_station_name, station->_outAddress, station->_innerAddress);\
			}


	}
}



#endif
