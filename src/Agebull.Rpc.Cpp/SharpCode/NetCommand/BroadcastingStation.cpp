/**
 * ZMQ�㲥������
 *
 *
 */

#include "stdafx.h"
#include "BroadcastingStation.h"
#include "StationWarehouse.h"

namespace agebull
{
	namespace zmq_net
	{
		/**
		*@brief ������Ϣ
		*/
		bool BroadcastingStation::publish(string station, string publiher, string title, string arg)
		{
			auto pub = StationWarehouse::find(station);
			if (pub == nullptr || pub->get_station_type() != STATION_TYPE_PUBLISH)
				return false;
			return static_cast<BroadcastingStationBase*>(pub)->publish(publiher, title, arg);
		}

		/**
		*@brief ������Ϣ
		*/
		bool BroadcastingStationBase::publish(string publiher, string title, string arg) const
		{
			//boost::lock_guard<boost::mutex> guard(_mutex);
			if (!can_do() || publiher.length() == 0)
				return false;
			vector<sharp_char> response;
			vector<string> argument;
			argument.push_back(title);
			argument.push_back(arg);
			RequestSocket<ZMQ_REQ, false> socket(publiher.c_str(), _station_name.c_str());
			return socket.request(argument, response);
		}

		/**
		* @brief ��������
		*/
		void BroadcastingStationBase::request(ZMQ_HANDLE socket)
		{
			vector<sharp_char> list;
			//0 ·�ɵ��ĵ�ַ 1 ��֡ 2 ���� 3 ����
			_zmq_state = recv(socket, list);
			if (_zmq_state != ZmqSocketState::Succeed)
			{
				log_error3("������Ϣʧ��%s(%d)%s", _station_name.c_str(), _inner_port, state_str(_zmq_state));
				return;
			}
			acl::string str(list[2]);
			str.append(" ").append(list[0]).append(" ").append(list[3]);

			_zmq_state = send_late(_inner_socket, str.c_str());

		}
		SystemMonitorStation* SystemMonitorStation::example = nullptr;
	}
}
