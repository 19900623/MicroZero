using System;
using System.Collections.Generic;
using System.Linq;
using Agebull.Common.Logging;
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
        ///     �ر��׽���
        /// </summary>
        public static void CloseSocket(this NetMQSocket socket,string address)
        {
            if (socket == null)
                return;
            try
            {
                socket.Disconnect(address);
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e); //һ�����޷����ӷ�����������
            }
            socket.Close();
            socket.Dispose();
            socket = null;
        }
        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="datas"></param>
        /// <param name="tryCnt"></param>
        /// <returns></returns>
        public static bool ReceiveString(this NetMQSocket request, out List<string> datas, int tryCnt = 3)
        {
            datas = new List<string>();

            var more = true;
            var cnt = 0;
            var ts = new TimeSpan(0, 0, 3);
            //������Ϣ
            while (more)
            {
                if (!request.TryReceiveFrameString(ts, out var data, out more))
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
        public static string ReceiveString(NetMQSocket request, int tryCnt = 3)
        {
            return ReceiveString(request, out var datas, tryCnt) ? datas.FirstOrDefault() : null;
        }

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        public static bool SendString(this NetMQSocket request, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            return SendStringInner(request,args);
        }

        /// <summary>
        ///     �����ı�
        /// </summary>
        /// <param name="request"></param>
        /// <param name="args"></param>
        /// <returns></returns>
        private static bool SendStringInner(NetMQSocket request, params string[] args)
        {
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
        public static string Request(this NetMQSocket request, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            if (!SendString(request, args))
                throw new Exception("����ʧ��");
            return ReceiveString(request);
        }


        /// <summary>
        ///     ����һ������
        /// </summary>
        /// <param name="address">�����ַ</param>
        /// <param name="args">�������</param>
        /// <returns></returns>
        public static string RequestNet(this string address, params string[] args)
        {
            if (args.Length == 0)
                throw new ArgumentException("args ����Ϊ��");
            using (var request = new RequestSocket())
            {
                request.Options.Identity = StationProgram.Config.RealName;
                request.Options.ReconnectInterval = new TimeSpan(0, 0, 1);
                request.Options.DisableTimeWait = true;
                request.Connect(address);

                if (!SendStringInner(request, args))
                    throw new Exception($"{address}:����ʧ��");
                var res = ReceiveString(request, out var datas, 1) ? datas.FirstOrDefault() : null;
                request.Disconnect(address);
                return res;
            }
        }
    }
}