#include "../stdinc.h"
#include "zero_config.h"
#include "../ext/shared_char.h"
#include "../cfg/json_config.h"

namespace agebull
{
	namespace zero_net
	{
		int station_config::check_worker(station_worker& worker)
		{
			if (worker.state == 5)
				return -1;
			var ms = boost::posix_time::microsec_clock::local_time() - worker.pre_time;
			var tm = ms.total_milliseconds() / global_config::worker_sound_ivl;
			

			if (tm <= 1)
			{
				worker.state = 1;
				return worker.level = 5;
			}
			if (tm <= 2)
			{
				worker.state = 2;
				return worker.level = 4;
			}
			if (tm <= 4)
			{
				worker.state = 2;
				return worker.level = 3;
			}
			worker.state = 3;
			if (tm <= 8)
			{
				return worker.level = 2;
			}
			if (tm <= 16)
			{
				return worker.level = 1;
			}
			if (tm <= 32)
			{
				return worker.level = 0;
			}
			return worker.level = -1;
		}


		void station_config::worker_join(const char* identity, const char* ip)
		{
			boost::lock_guard<boost::mutex> guard(mutex);
			bool hase = false;
			for (station_worker& iter : workers)
			{
				if (strcmp(iter.identity, identity) == 0)
				{
					if (iter.state > 2)//����ʧ��
						log("worker_join*reincarnation ", identity);
					iter.active();
					hase = true;
					break;
				}
			}
			if (!hase)
			{
				station_worker wk;
				memset(wk.identity, 0, sizeof(wk.identity));
				strcpy(wk.identity, identity);
				wk.state = 1;
				wk.level = 5;
				wk.active();
				workers.push_back(wk);
				++ready_works_;
				log("worker_join", identity);
			}
		}


		void station_config::worker_heartbeat(const char* identity)
		{
			boost::lock_guard<boost::mutex> guard(mutex);
			for (station_worker& iter : workers)
			{
				if (strstr(iter.identity, identity) != nullptr)
				{
					//if (iter.state > 2)//����ʧ��
					//	log("worker_ready*reincarnation ", iter.identity);
					iter.active();
				}
				else
				{
					check_worker(iter);
				}
			}
		}
		void station_config::worker_ready(const char* identity)
		{
			boost::lock_guard<boost::mutex> guard(mutex);
			bool hase = false;
			for (station_worker& iter : workers)
			{
				if (strcmp(iter.identity, identity) == 0)
				{
					if (iter.state > 2)//����ʧ��
						log("worker_ready*reincarnation ", identity);
					iter.active();
					hase = true;
					break;
				}
			}
			if (!hase)
			{
				station_worker wk;
				memset(&wk, 0, sizeof(wk));
				strcpy(wk.identity, identity);
				wk.active();
				workers.push_back(wk);
				log("worker_ready", identity);
			}
		}

		/**
		* \brief վ������
		*/
		station_worker* station_config::get_workers()
		{
			{
				boost::lock_guard<boost::mutex> guard(mutex);
				int size = static_cast<int>(workers.size());
				if (size == 0)
				{
					worker_idx_ = -1;
					return nullptr;
				}
				if (size == 1)
				{
					worker_idx_ = 0;
					if (workers[0].state < 5)
						return &workers[0];
					workers.erase(workers.begin());
					return nullptr;
				}
				if (++worker_idx_ >= size)
				{
					worker_idx_ = 0;
				}
				if (workers[worker_idx_].state < 5)
					return &workers[worker_idx_];
				workers.erase(workers.begin() + worker_idx_);
			}
			return get_workers();
		}
		void station_config::worker_left(const char* identity)
		{
			if (workers.size() == 0)
				return;

			boost::lock_guard<boost::mutex> guard(mutex);
			vector<station_worker> array = workers;
			for (int i = static_cast<int>(array.size()) - 1; i >= 0; --i)
			{
				if (strstr(array[i].identity, identity) != nullptr)
				{
					workers.erase(workers.begin() + i);
					log("worker_left", array[i].identity);
				}
			}
		}

		void station_config::check_works()
		{
			if (workers.size() == 0)
				return;
			boost::lock_guard<boost::mutex> guard(mutex);
			ready_works_ = 0;
			vector<station_worker> array = workers;
			for (int i = static_cast<int>(array.size()) - 1; i >= 0; --i)
			{
				check_worker(workers[i]);
				if (workers[i].state <= 2)
				{
					++ready_works_;
				}
				else if (workers[i].level < 1)
				{
					log("station_worker failed", array[i].identity);
					workers.erase(workers.begin() + i);
				}
			}
		}
		const char* fields[] =
		{
			"name"
			, "station_type"
			, "request_port"
			, "worker_out_port"
			, "worker_in_port"
			, "description"
			, "caption"
			//, "station_alias"
			, "station_state"
			, "request_in"
			, "request_out"
			, "request_err"
			, "request_deny"
			, "worker_in"
			, "worker_out"
			, "worker_err"
			, "worker_deny"
			//, "short_name"
		};
		enum class config_fields
		{
			station_name
			, station_type
			, request_port
			, worker_out_port
			, worker_in_port
			, description
			, caption
			//, station_alias
			, station_state
			, request_in
			, request_out
			, request_err
			, request_deny
			, worker_in
			, worker_out
			, worker_err
			, worker_deny
			//, short_name
		};
		void station_config::read_json(const char* val)
		{
			boost::lock_guard<boost::mutex> guard(mutex);
			acl::json json;
			json.update(val);
			acl::json_node* iter = json.first_node();
			while (iter)
			{
				const char* tag = iter->tag_name();
				if (tag == nullptr || tag[0] == 0 || iter->is_null())
				{
					iter = json.next_node();
					continue;
				}

				const int idx = strmatchi(tag, fields);
				switch (static_cast<config_fields>(idx))
				{
				case config_fields::station_name:
					station_name = iter->get_string();
					break;
				case config_fields::station_type:
					station_type = json_read_int(iter);
					break;
				//case config_fields::short_name:
				//	short_name = iter->get_string();
				//	break;
				case config_fields::description:
					station_description = iter->get_string();
					break;
				case config_fields::caption:
					station_caption = iter->get_string();
					break;
				//case config_fields::station_alias:
				//	alias.clear();
				//	{
				//		var ch = iter->first_child();
				//		var iter_arr = ch->first_child();
				//		while (iter_arr)
				//		{
				//			auto txt = iter_arr->get_text();
				//			if (txt != nullptr)
				//				alias.emplace_back(txt);
				//			iter_arr = ch->next_child();
				//		}
				//	}
				//	break;
				case config_fields::request_port:
					request_port = json_read_int(iter);
					break;
				case config_fields::worker_out_port:
					worker_out_port = json_read_int(iter);
					break;
				case config_fields::worker_in_port:
					worker_in_port = json_read_int(iter);
					break;
				case config_fields::station_state:
					station_state_ = static_cast<station_state>(json_read_num(iter));
					break;
				case config_fields::request_in:
					request_in = json_read_num(iter);
					break;
				case config_fields::request_out:
					request_out = json_read_num(iter);
					break;
				case config_fields::request_err:
					request_err = json_read_num(iter);
					break;
				case config_fields::request_deny:
					request_deny = json_read_num(iter);
					break;
				case config_fields::worker_err:
					worker_err = json_read_num(iter);
					break;
				case config_fields::worker_in:
					worker_in = json_read_num(iter);
					break;
				case config_fields::worker_out:
					worker_out = json_read_num(iter);
					break;
				case config_fields::worker_deny:
					worker_deny = json_read_num(iter);
					break;
				default: break;
				}
				iter = json.next_node();
			}
			check_type_name();
		}

		/**
		* \brief д��JSON
		* \param type ��¼���� 0 ȫ�� 1 ��̬��Ϣ 2 ������Ϣ
		*/
		acl::string station_config::to_json(int type)
		{
			boost::lock_guard<boost::mutex> guard(mutex);
			acl::json json;
			acl::json_node& node = json.create_node();

			json_add_str(node, "name", station_name);
			//վ�������Ϣ,��������״̬��
			if (type != 2)
			{
				json_add_num(node, "station_type", station_type);
				json_add_num(node, "request_port", request_port);
				json_add_num(node, "worker_in_port", worker_in_port);
				json_add_num(node, "worker_out_port", worker_out_port);
				json_add_bool(node, "is_base", is_base);
				json_add_str(node, "description", station_description);
				//node.add_text("short_name", short_name.empty() ? station_name.c_str() : short_name.c_str());
				//if (alias.size() > 0)
				//{
				//	acl::json_node& array = json.create_array();
				//	for (auto alia : alias)
				//	{
				//		json_add_array_str(array, alia);
				//	}
				//	node.add_child("station_alias", array);
				//}
			}
			//վ�����,�������ڻ�����Ϣ��
			if (type != 1)
			{
				json_add_num(node, "request_in", request_in);
				json_add_num(node, "request_out", request_out);
				json_add_num(node, "request_err", request_err);
				json_add_num(node, "request_deny", request_deny);

				json_add_num(node, "worker_in", worker_in);
				json_add_num(node, "worker_out", worker_out);
				json_add_num(node, "worker_err", worker_err);
				json_add_num(node, "worker_deny", worker_deny);
			}
			//����Ĺ���վ����Ϣ,������״̬
			if (type >= 2 && workers.size() > 0)
			{
				acl::json_node& array = json.create_array();
				for (auto& station_worker : workers)
				{
					acl::json_node& work = json.create_node();
					json_add_chr(work, "real_name", station_worker.identity);
					json_add_num(work, "level", station_worker.level);
					json_add_num(work, "state", station_worker.state);
					json_add_num(work, "pre_time", to_time_t(station_worker.pre_time));
					json_add_str(work, "ip_address", station_worker.ip_address);
					array.add_child(work);
				}
				node.add_child("workers", array);
			}

			json_add_num(node, "station_state", static_cast<int>(station_state_));
			return node.to_string();
		}
	}
}
