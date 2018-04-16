using System;
using System.Collections.Generic;
using System.Linq;
using NetMQ;
using NetMQ.Sockets;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public static class ZeroHelper
    {

        /// <summary>
        /// ����״̬
        /// </summary>
        public const byte zero_status_success = (byte)'+';
        /// <summary>
        /// ����״̬
        /// </summary>
        public const byte zero_status_bad = (byte)'-';

        /// <summary>
        /// �ɹ�
        /// </summary>
        public const string zero_command_ok = "+ok";
        /// <summary>
        /// �ƻ�ִ��
        /// </summary>
        public const string zero_command_plan = "+plan";
        public const string zero_command_error = "-error";
        public const string zero_command_runing = "+runing";
        public const string zero_command_bye = "+bye";
        public const string zero_command_wecome = "+wecome";
        public const string zero_vote_sended = "+send";
        public const string zero_vote_closed = "+close";
        public const string zero_vote_bye = "+bye";
        public const string zero_vote_waiting = "+waiting";
        public const string zero_vote_start = "+start";

        public const string zero_vote_end = "+end";
        //����״̬
        public const string zero_command_no_find = "-no find";

        public const string zero_command_invalid = "-invalid";
        //����״̬
        public const string zero_command_no_support = "-no support";
        public const string zero_command_failed = "-failes";
        public const string zero_command_arg_error = "-ArgumentError! must like : call[name][command][argument]";
        public const string zero_command_install_arg_error = "-ArgumentError! must like :install [type] [name]";
        public const string zero_command_timeout = "-time out";
        public const string zero_command_net_error = "-net error";
        public const string zero_command_not_worker = "-not work";
        public const string zero_api_unknow_error = "-error";
        public const string zero_vote_unknow_error = "-error";

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="datas"></param>
        /// <param name="tryCnt"></param>
        /// <returns></returns>
        public static bool ReceiveString(this RequestSocket request, out List<string> datas, int tryCnt = 3)
        {
            datas = new List<string>();

            var more = true;
            var cnt = 0;
            //������Ϣ
            while (more)
            {
                if (!request.TryReceiveFrameString(new TimeSpan(0, 0, 3), out var data, out more))
                {
                    if (++cnt >= tryCnt)
                        return false;
                    more = true;
                }
                datas.Add(data);
            }
            return true;
        }

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="tryCnt"></param>
        /// <returns></returns>
        public static string ReceiveString(RequestSocket request, int tryCnt = 3)
        {
            return ReceiveString(request, out var datas, tryCnt) ? datas.FirstOrDefault() : null;
        }

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        public static bool SendString(this RequestSocket request, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            var i = 0;
            for (; i < args.Length - 1; i++)
                request.SendFrame(args[i] ?? "", true);
            return request.TrySendFrame(args[i] ?? "");
        }

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        public static string Request(this RequestSocket request, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            if (!SendString(request, args))
                throw new Exception("����ʧ��");
            return ReceiveString(request);
        }


        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="address"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        public static string RequestNet(this string address, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            using (var request = new RequestSocket())
            {
                request.Options.Identity = StationProgram.Config.StationName.ToAsciiBytes();
                request.Options.ReconnectInterval = new TimeSpan(0, 0, 3);
                request.Connect(address);
                var res = Request(request, args);
                request.Disconnect(address);
                return res;
            }
        }
    }
}