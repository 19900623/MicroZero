#include "stdafx.h"
#include "ZeroStation.h"

#define port_redis_key "net:port:next"

namespace agebull
{
	namespace zmq_net
	{


		/**
		* @brief ��ʼ��
		*/
		
		bool ZeroStation::initialize()
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
		
		bool ZeroStation::destruct()
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
		
		bool ZeroStation::poll()
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
		/**
		* @brief ��ͣ
		*/
		
		bool ZeroStation::pause(bool waiting)
		{
			if (station_state::Run != _station_state)
				return false;
			_station_state = station_state::Pause;
			monitor(_station_name, "station_pause", "");
			return true;
		}

		/**
		* @brief ����
		*/
		
		bool ZeroStation::resume(bool waiting)
		{
			if (station_state::Pause != _station_state)
				return false;
			_station_state = station_state::Run;
			_state_semaphore.post();
			monitor(_station_name, "station_resume", "");
			return true;
		}

		/**
		* @brief ����
		*/
		
		bool ZeroStation::close(bool waiting)
		{
			if (!can_do())
				return false;
			_station_state = station_state::Closing;
			while (waiting && _station_state == station_state::Closing)
				thread_sleep(1000);
			monitor(_station_name, "station_closing", "");
			return true;
		}

	}
}