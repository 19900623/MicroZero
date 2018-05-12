#pragma once
#ifndef _ZERO_STATION_H
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
		class zero_station
		{
			friend class station_warehouse;
		protected:
			/**
			* \brief ����ƻ���ѯ״̬
			*/
			bool in_plan_poll_;

			/**
			* \brief ����
			*/
			acl::string config_;
			/**
			* \brief ״̬�ź���
			*/
			boost::interprocess::interprocess_semaphore state_semaphore_;
			/**
			* \brief API��������
			*/
			string station_name_;

			/**
			* \brief վ������
			*/
			int station_type_;

			/**
			* \brief �ⲿ��ַ
			*/
			int request_port_;

			/**
			* \brief ������ַ
			*/
			int response_port_;

			/**
			* \brief ������ַ
			*/
			int heart_port_;
			/**
			* \brief �ⲿSOCKET����
			*/
			int request_zmq_type_;

			/**
			* \brief ����SOCKET����
			*/
			int response_zmq_type_;

			/**
			* \brief ����SOCKET����
			*/
			int heart_zmq_type_;
			/*
			*\brief ��ѯ�ڵ�
			*/
			zmq_pollitem_t* poll_items_;
			/*
			*\brief �ڵ�����
			*/
			int poll_count_;
			/**
			* \brief ���þ��
			*/
			void* request_scoket_;
			/**
			* \brief ���þ��
			*/
			void* request_socket_inproc_;
			/**
			* \brief �������
			*/
			void* response_socket_;
			/**
			* \brief �������
			*/
			void* heart_socket_;
			/**
			* \brief ��ǰZMQִ��״̬
			*/
			zmq_socket_state zmq_state_;
			/**
			* \brief ��ǰվ��״̬
			*/
			station_state station_state_;
		public:
			/**
			* \brief ȡ����������
			*/
			const char* get_config() const
			{
				return config_.c_str();
			}
			/**
			* \brief ��ǰZMQִ��״̬
			*/
			zmq_socket_state get_zmq_state() const
			{
				return zmq_state_;
			}
			/**
			* \brief API��������
			*/
			int get_station_type() const
			{
				return station_type_;
			}
			/**
			* \brief ��ǰվ��״̬
			*/
			station_state get_station_state() const
			{
				return station_state_;
			}
			/**
			* \brief API��������
			*/
			const string& get_station_name() const
			{
				return station_name_;
			}

			/**
			* \brief �ⲿ��ַ
			*/
			string get_out_address()const
			{
				string addr("tcp://*:");
				addr += std::to_string(request_port_);
				return addr;
			}

			/**
			* \brief ������ַ
			*/
			string get_inner_address()const
			{
				string addr("tcp://*:");
				addr += std::to_string(response_port_);
				return addr;
			}

			/**
			* \brief ������ַ
			*/
			string get_heart_address()const
			{
				string addr("tcp://*:");
				addr += std::to_string(heart_port_);
				return addr;
			}

			/**
			* \brief ����
			*/
			zero_station(string name, int type, int request_zmq_type, int response_zmq_type, int heart_zmq_type)
				: in_plan_poll_(false)
				, state_semaphore_(1)
				, station_name_(std::move(name))
				, station_type_(type)
				, request_port_(0)
				, response_port_(0)
				, heart_port_(0)
				, request_zmq_type_(request_zmq_type)
				, response_zmq_type_(response_zmq_type)
				, heart_zmq_type_(heart_zmq_type)
				, poll_items_(nullptr)
				, poll_count_(0)
				, request_scoket_(nullptr)
				, request_socket_inproc_(nullptr)
				, response_socket_(nullptr)
				, heart_socket_(nullptr)
				, zmq_state_(zmq_socket_state::Succeed)
				, station_state_(station_state::None)
			{
			}
			/**
			* \brief �������ڵ��ڵ�����
			*/
			void load_now(vector<plan_message>& messages) const;

			/**
			* \brief ɾ��һ���ƻ�
			*/
			bool remove_next(plan_message& message) const;

			/**
			* \brief �ƻ���һ��ִ��ʱ��
			*/
			bool plan_next(plan_message& message, bool first) const;

			/**
			* \brief ������һ��ִ��ʱ��
			*/
			bool save_next(plan_message& message) const;

			/**
			* \brief ������Ϣ
			*/
			bool save_message(plan_message& message) const;

			/**
			* \brief ��ȡ��Ϣ
			*/
			bool load_message(uint id, plan_message& message) const;

			/**
			* \brief ɾ����Ϣ
			*/
			bool remove_message(plan_message& message) const;

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
			bool send_data(vector<sharp_char>& datas,int first_index=0)
			{
				zmq_state_ = send(response_socket_, datas, first_index);
				return zmq_state_ == zmq_socket_state::Succeed;
			}
			/**
			* \brief ����
			*/
			virtual ~zero_station()
			{
				zero_station::close(true);
				station_state_ = station_state::Destroy;
			}
			/**
			* \brief �ܷ��������
			*/
			virtual bool can_do() const
			{
				return (station_state_ == station_state::Run || station_state_ == station_state::Pause) && get_net_state() == NET_STATE_RUNING;
			}
			/**
			* \brief ����Ƿ���ͣ
			*/
			bool check_pause()
			{
				if (station_state_ == station_state::Pause)
				{
					state_semaphore_.timed_wait(time_span(1000));
					return station_state_ == station_state::Pause;
				}
				return false;
			}
			/**
			* \brief ��ʼ��
			*/
			bool initialize();

			/**
			* \brief ����
			*/
			bool do_initialize();
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
				auto station = static_cast<zero_station*>(arg);
				station->plan_poll();
			}
		protected:

			/**
			* \brief ����ƻ�
			*/
			void save_plan(ZMQ_HANDLE socket, vector<sharp_char> list) const
			{
				plan_message message;
				message.request_caller = list[0];
				for (size_t idx = 3;idx < list.size();idx++)
				{
					message.messages.push_back(list[idx]);
				}
				message.read_plan(list[0].get_buffer());
				plan_next(message, true);
			}
			/**
			* \brief �ƻ���ѯ
			*/
			void plan_poll();

			/**
			* \brief �������ϵ���Ӧ
			*/
			virtual void response() {}
			/**
			* \brief ���ü��ϵ���Ӧ
			*/
			virtual void request(ZMQ_HANDLE socket,bool inner) = 0;
			/**
			* ��������Ӧ
			*/
			virtual void heartbeat(){}
		};

	}
}
#endif
