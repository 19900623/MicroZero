using System;
using System.Diagnostics;
using System.Threading;
using Agebull.Common.Logging;
using NetMQ;
using NetMQ.Sockets;
using Yizuan.Service.Api;

namespace Agebull.Zmq.Rpc
{
    /// <summary>
    ///     �ʴ�ʽ�����
    /// </summary>
    public class ZmqHeartbeatPump : ZmqCommandPump<RequestSocket>
    {
        /// <summary>
        /// ��������
        /// </summary>
        public string ServiceName { get; set; }
        /// <summary>
        /// ����SOCKET����
        /// </summary>
        /// <remarks>�汾�����ݵ��¸ı�</remarks>
        protected override RequestSocket CreateSocket()
        {
            return new RequestSocket();
        }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        protected sealed override void DoInitialize()
        {

        }

        /// <summary>
        /// ����Socket����
        /// </summary>
        /// <param name="socket"></param>
        protected sealed override void OptionSocktet(RequestSocket socket)
        {
            //�û�ʶ��
            socket.Options.Identity = ServiceName.ToAsciiBytes();
            //����Ϣֻ����������ɵ�����
            //�ر�����ͣ��ʱ��,����
            socket.Options.Linger = new TimeSpan(0, 0, 0, 0, 50);
            socket.Options.ReceiveLowWatermark = 500;
            socket.Options.SendLowWatermark = 500;
        }

        /// <summary>
        /// ִ�й���
        /// </summary>
        /// <param name="socket"></param>
        /// <returns>����״̬,����-1�ᵼ������</returns>
        protected sealed override int DoWork(RequestSocket socket)
        {
            Thread.Sleep(5000);
            var result = SendCommand(socket,"MAMA", ApiContext.MyAddress,true);
            return result == "Failed" || "Exception" == result ? -1 : 0;
        }

        /// <summary>
        /// ���Ӵ���
        /// </summary>
        /// <param name="socket"></param>
        protected sealed override void OnDeConnected(RequestSocket socket)
        {
            var result =  SendCommand(socket, "LAOWANG", ApiContext.MyAddress, true);
            Console.WriteLine(result);
        }
    }
}