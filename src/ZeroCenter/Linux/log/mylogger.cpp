#include <boost/lexical_cast.hpp>
#include "mylogger.h"
using namespace std;

//void myprintf(string msg,const char *fm, va_list ap)
//{
//	const char* fmt = fm;
//	char *s;
//	char flag;
//	while (*fmt) {
//		flag = *fmt++;
//		if (flag != '%') {
//			msg += boost::lexical_cast<string>(flag);
//			continue;
//		}
//		flag = *fmt++;//记得后移一位
//		switch (flag)
//		{
//		case 's':
//			s = va_arg(ap, char*);
//			msg.append(s);
//			break;
//		case 'd': /* int */
//			msg += boost::lexical_cast<string>(va_arg(ap, int));
//			break;
//		case 'f': /* double*/
//			msg += boost::lexical_cast<string>(va_arg(ap, double));
//			break;
//		case 'c': /* char*/
//		default:
//			msg += boost::lexical_cast<string>(va_arg(ap, char));
//			break;
//		}
//	}
//}

namespace agebull
{
	///C端命令调用队列锁
	boost::mutex server_cmd_mutex;

	/*
	* 输出到DEBUG窗口
	*/
	inline void out_debug(string msg)
	{
//		msg = "\r\n" + msg;
//#ifdef WIN32
//		OutputDebugStringA(msg.c_str());
//#endif
		boost::lock_guard<boost::mutex> guard(server_cmd_mutex);
		cout << endl << msg.c_str();
	}
	void log_acl_msg(const char* msg)
	{
		{
			out_debug(msg);
		}
		acl::log::msg1(msg);
	}
	void log_acl_msg(const string& msg)
	{
		{
			out_debug(msg);
		}
		acl::log::msg1(msg.c_str());
	}
	void log_acl_warn(const char* fname, int line, const char* func, const string& msg)
	{
		out_debug(msg);
		acl::log::warn4(fname, line, func, msg.c_str());
	}
	void log_acl_error(const char* fname, int line, const char* func, const string& msg)
	{
		{
			out_debug(msg);
		}
		acl::log::error4(fname, line, func, msg.c_str());
	}
	void log_acl_fatal(const char* fname, int line, const char* func, const string& msg)
	{
		acl::log::fatal4(fname, line, func, msg.c_str());
	}
	void log_acl_debug(int section, int  level, const char* fname, int line, const char* func, const std::string& msg)
	{
		//if (level < 2)
		{
			out_debug(msg);
		}
		acl::log::msg6(section, level, fname, line, func, msg.c_str());
	}
	void log_acl_trace(int section, int  level, const string& msg)
	{
		//if (level < 2)
		{
			out_debug(msg);
		}
		acl::log::msg1(msg.c_str());
	}
}