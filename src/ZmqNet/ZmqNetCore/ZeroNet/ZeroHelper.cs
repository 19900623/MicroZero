using System;
using System.Collections.Generic;
using System.Linq;
using NetMQ;
using NetMQ.Sockets;

namespace ZmqNet.Rpc.Core.ZeroNet
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public static class ZeroHelper
    {

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="datas"></param>
        /// <param name="try_cnt"></param>
        /// <returns></returns>
        public static bool ReceiveString(this RequestSocket request, out List<string> datas, int try_cnt = 3)
        {
            datas = new List<string>();

            var more = true;
            var cnt = 0;
            //������Ϣ
            while (more)
            {
                string data;
                if (!request.TryReceiveFrameString(new TimeSpan(0, 0, 3), out data, out more))
                {
                    if (++cnt >= try_cnt)
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
        /// <param name="try_cnt"></param>
        /// <returns></returns>
        public static string ReceiveString(RequestSocket request, int try_cnt = 3)
        {
            List<string> datas;
            return ReceiveString(request, out datas) ? datas.FirstOrDefault() : null;
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
                return "";
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
                request.Options.ReconnectInterval = new TimeSpan(0, 10, 59);
                request.Connect(address);
                return Request(request, args);
            }
        }
    }
}