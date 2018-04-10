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
        /// ��ֹ����
        /// </summary>
        public const byte zero_end = (byte)'?';
        /// <summary>
        /// ִ�мƻ�
        /// </summary>
        public const byte zero_plan = (byte)'@';
        /// <summary>
        /// ����
        /// </summary>
        public const byte zero_arg = (byte)'$';
        /// <summary>
        /// ����ID
        /// </summary>
        public const byte zero_request_id = (byte)':';
        /// <summary>
        /// ������/������
        /// </summary>
        public const byte zero_requester = (byte)'>';
        /// <summary>
        /// ������/������
        /// </summary>
        public const byte zero_pub_publisher = zero_requester;
        /// <summary>
        /// �ظ���/�˷���
        /// </summary>
        public const byte zero_responser = (byte)'<';
        /// <summary>
        /// ������/�˷���
        /// </summary>
        public const byte zero_pub_subscriber = zero_responser;
        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte zero_pub_title = (byte)'*';
        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte zero_pub_sub = (byte)'&';
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
                return Request(request, args);
            }
        }
    }
}