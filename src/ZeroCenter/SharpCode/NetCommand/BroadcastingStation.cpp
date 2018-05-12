/**
 * ZMQ�㲥������
 *
 *
 */

#include "stdafx.h"
#include <NetCommand/BroadcastingStation.h>
#include <NetCommand/StationWarehouse.h>

namespace agebull
{
	namespace zmq_net
	{
		/**
		 * \brief ����
		 */
		system_monitor_station* system_monitor_station::example_ = nullptr;
		/**
		* \brief ��������
		*/
		void broadcasting_station_base::request(ZMQ_HANDLE socket, bool inner)
		{
			vector<sharp_char> list;
			zmq_state_ = recv(socket, list);
			if (zmq_state_ != zmq_socket_state::Succeed)
			{
				log_error3("������Ϣʧ��%s(%d)%s", station_name_.c_str(), response_port_, state_str(zmq_state_));
				return;
			}
			const sharp_char& caller = list[0];
			if (list.size() <= 3)
			{
				zmq_state_ = send_addr(socket, *caller);
				zmq_state_ = send_late(socket, ZERO_STATUS_FRAME_INVALID);
				return;
			}
			sharp_char& title = list[2];
			sharp_char& description = list[3];
			char* const buf = description.get_buffer();
			const size_t list_size = list.size();
			const auto frame_size = static_cast<size_t>(buf[0]);
			if (frame_size >= description.size() || (frame_size + 4) != list_size)
			{
				zmq_state_ = send_addr(socket, *caller);
				zmq_state_ = send_late(socket, ZERO_STATUS_FRAME_INVALID);
				return;
			}
			bool hase_plan = false;
			for (size_t idx = 1; idx <= frame_size; idx++)
			{
				if (buf[idx] == ZERO_FRAME_PLAN)
				{
					hase_plan = true;
					break;
				}
			}
			if (!hase_plan)
			{
				zmq_state_ = send_addr(socket, *caller);
				zmq_state_ = send_late(socket, ZERO_STATUS_OK);
				send_data(list, 2);
				//cout << *caller << endl;
				return;
			}
			zmq_state_ = send_addr(socket, *caller);
			zmq_state_ = send_late(socket, ZERO_STATUS_PLAN);

			plan_message message;
			message.messages.push_back(title);
			message.messages.push_back(description);
			sharp_char plan, sub, arg, publisher, id;
			for (size_t idx = 1; idx <= frame_size; idx++)
			{
				switch (buf[idx])
				{
				case ZERO_FRAME_PLAN:
					plan = list[3 + idx];
					break;
				case ZERO_FRAME_PUBLISHER:
					publisher = list[3 + idx];
					break;
				case ZERO_FRAME_SUBTITLE:
					sub = list[3 + idx];
					break;
				case ZERO_FRAME_ARG:
					arg = list[3 + idx];
					break;
				case ZERO_FRAME_REQUEST_ID:
					id = list[3 + idx];
					break;
				}
			}
			int cnt = 0;
			buf[++cnt] = ZERO_FRAME_PUBLISHER;
			message.messages.push_back(publisher);
			if (!id.empty())
			{
				buf[++cnt] = ZERO_FRAME_REQUEST_ID;
				message.messages.push_back(id);
			}
			if (!sub.empty())
			{
				buf[++cnt] = ZERO_FRAME_SUBTITLE;
				message.messages.push_back(sub);
			}
			if (!arg.empty())
			{
				buf[++cnt] = ZERO_FRAME_ARG;
				message.messages.push_back(arg);
			}
			buf[0] = cnt;

			message.read_plan(plan.get_buffer());
			message.request_caller = caller;
			message.messages_description = description;
			plan_next(message, true);


		}

		/**
		*\brief ������Ϣ
		*/
		bool broadcasting_station_base::publish(const sharp_char& title, const sharp_char& description, vector<sharp_char>& datas)
		{
			send_more(response_socket_, *title);
			send_more(response_socket_, *description);
			zmq_state_ = send(response_socket_, datas);
			return zmq_state_ == zmq_socket_state::Succeed;
		}

		/**
		*\brief ������Ϣ
		*/
		bool broadcasting_station::publish(const string& station, const string& publiher, 
			const string& title, const string& sub, const string& arg)
		{
			const auto pub = station_warehouse::instance(station);
			if (pub == nullptr || pub->get_station_type() != STATION_TYPE_PUBLISH)
				return false;
			return dynamic_cast<broadcasting_station_base*>(pub)->publish(publiher, title, sub, arg);
		}

		/**
		*\brief ������Ϣ
		*/
		bool broadcasting_station_base::publish(const string& publiher, const string& title, const string& arg)
		{
			//boost::lock_guard<boost::mutex> guard(_mutex);
			if (!can_do() || publiher.length() == 0)
				return false;
			sharp_char description;
			description.alloc(6);
			char* buf = description.get_buffer();
			buf[0] = 2;
			buf[1] = ZERO_FRAME_PUBLISHER;
			buf[2] = ZERO_FRAME_ARG;
			buf[3] = ZERO_FRAME_END;
			vector<sharp_char> datas;
			datas.emplace_back(title.c_str());
			datas.emplace_back(description);
			datas.emplace_back(publiher.c_str());
			datas.emplace_back(arg.c_str());
			return send_data(datas);
		}
		/**
		*\brief ������Ϣ
		*/
		bool broadcasting_station_base::publish(const string& publiher, const string& title, const string& sub, const string& arg)
		{
			//boost::lock_guard<boost::mutex> guard(_mutex);
			if (!can_do() || publiher.empty())
				return false;
			sharp_char description;
			description.alloc(6);
			char* buf = description.get_buffer();
			buf[0] = 3;
			buf[1] = ZERO_FRAME_PUBLISHER;
			buf[2] = ZERO_FRAME_SUBTITLE;
			buf[3] = ZERO_FRAME_ARG;
			buf[4] = ZERO_FRAME_END;
			vector<sharp_char> datas;
			datas.emplace_back(title.c_str());
			datas.push_back(description);
			datas.emplace_back(publiher.c_str());
			datas.emplace_back(sub.c_str());
			datas.emplace_back(arg.c_str());

			return send_data(datas);
		}

		/**
		*\brief ������Ϣ
		*/
		bool broadcasting_station_base::publish(const string& publiher, const string& title, const string& sub, const string& plan, const string& arg) const
		{
			//boost::lock_guard<boost::mutex> guard(_mutex);
			if (!can_do() || publiher.length() == 0)
				return false;
			plan_message message;
			message.request_caller = publiher;
			vector<sharp_char> datas;
			vector<string> argument;
			message.messages_description.alloc(6);
			char* buf = message.messages_description.get_buffer();
			buf[0] = 3;
			buf[1] = ZERO_FRAME_PUBLISHER;
			buf[2] = ZERO_FRAME_SUBTITLE;
			buf[3] = ZERO_FRAME_ARG;
			buf[4] = ZERO_FRAME_END;
			message.messages.emplace_back(title.c_str());
			datas.emplace_back(message.messages_description);
			datas.emplace_back(publiher.c_str());
			datas.emplace_back(sub.c_str());
			datas.emplace_back(arg.c_str());

			message.read_plan(plan.c_str());

			return plan_next(message, true);
		}

		/**
		*\brief ����һ���㲥�߳�
		*/
		void broadcasting_station::launch(void* arg)
		{
			auto station = static_cast<broadcasting_station*>(arg);
			if (!station_warehouse::join(station))
			{
				return;
			}
			if (station->zmq_state_ == zmq_socket_state::Succeed)
				log_msg4("%s(%d | %d) %d start.", station->station_name_.c_str(), station->request_port_, station->response_port_, station->station_type_);
			else
				log_msg4("%s(%d | %d) %d restart", station->station_name_.c_str(), station->request_port_, station->response_port_, station->station_type_);
			if (!station->initialize())
			{
				log_msg4("%s(%d | %d) %d con`t launch.", station->station_name_.c_str(), station->request_port_, station->response_port_, station->station_type_);
				return;
			}
			log_msg4("%s(%d | %d) %d runing", station->station_name_.c_str(), station->request_port_, station->response_port_, station->station_type_);

			zmq_threadstart(plan_poll, arg);
			const bool re_strart = station->poll();
			while (station->in_plan_poll_)
			{
				sleep(1);
			}
			//zmq_threadclose(t);
			station_warehouse::left(station);
			station->destruct();
			if (re_strart)
			{
				broadcasting_station* station2 = new broadcasting_station(station->station_name_);
				station2->zmq_state_ = zmq_socket_state::Again;
				zmq_threadstart(launch, station2);
			}
			else
			{
				log_msg3("%s(%d | %d) closed", station->station_name_.c_str(), station->request_port_, station->response_port_);
			}
			delete station;
		}


		void system_monitor_station::launch(void*)
		{
			example_ = new system_monitor_station();
			if (!station_warehouse::join(example_))
			{
				delete example_;
				return;
			}
			if (example_->zmq_state_ == zmq_socket_state::Succeed)
				log_msg4("%s(%d | %d) %d start.", example_->station_name_.c_str(), example_->request_port_, example_->response_port_, example_->station_type_);
			else
				log_msg4("%s(%d | %d) %d restart.", example_->station_name_.c_str(), example_->request_port_, example_->response_port_, example_->station_type_);
			if (!example_->initialize())
			{
				log_msg4("%s(%d | %d) %d con`t launch.", example_->station_name_.c_str(), example_->request_port_, example_->response_port_, example_->station_type_);
				return;
			}
			log_msg4("%s(%d | %d) %d runing", example_->station_name_.c_str(), example_->request_port_, example_->response_port_, example_->station_type_);
			zmq_threadstart(plan_poll, example_);
			const bool re_strart = example_->poll();
			while (example_->in_plan_poll_)
			{
				sleep(1);
			}
			//���͹ر���Ϣ
			acl::string msg;
			msg.format("station_end\r\n%s\r\n%d\r\n", example_->station_name_.c_str(), example_->response_port_);
			send_late(example_->response_socket_, msg.c_str());
			thread_sleep(100);
			example_->destruct();
			if (re_strart)
			{
				delete example_;
				example_ = nullptr;
				zmq_threadstart(launch, nullptr);
			}
			else
			{
				log_msg3("%s(%d | %d)closed", example_->station_name_.c_str(), example_->request_port_, example_->response_port_);

				delete example_;
				example_ = nullptr;
			}
		}
	}
}
