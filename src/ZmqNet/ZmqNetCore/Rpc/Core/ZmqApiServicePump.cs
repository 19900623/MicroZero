using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Agebull.Common.Logging;
using NetMQ;
using NetMQ.Sockets;
using Yizuan.Service.Api;

namespace Agebull.Zmq.Rpc
{
    /// <summary>
    ///     �ʴ�ʽ�����
    /// </summary>
    public class ZmqApiServicePump : ZmqCommandPump<RequestSocket>
    {
        #region ��Ϣ��

        public ZmqHeartbeatPump Heartbeat { get; private set; }
        /// <summary>
        /// ��������
        /// </summary>
        public string ServiceName => $"{ApiContext.MyServiceName}:{Thread.CurrentThread.ManagedThreadId}";

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
        /// ����
        /// </summary>
        protected sealed override void DoDispose()
        {
            Heartbeat.Dispose();
        }

        /// <summary>
        /// ���Ӵ���
        /// </summary>
        /// <param name="socket"></param>
        protected override void OnConnected(RequestSocket socket)
        {
            var result = SendCommand(socket, "READY", ApiContext.MyAddress, false);
            Console.WriteLine($"OnConnected => {ServiceName}:{result}");
            Heartbeat = new ZmqHeartbeatPump
            {
                ServiceName = ServiceName,
                ZmqAddress = RouteRpc.Singleton.Config.HeartbeatUrl
            };
            Heartbeat.Initialize();
            Heartbeat.Run();
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

        public class CallArgument
        {
            public RequestSocket socket;
            public string client;
            public string callArg;
        }
        /// <summary>
        /// ִ�й���
        /// </summary>
        /// <param name="socket"></param>
        /// <returns>����״̬,����-1�ᵼ������</returns>
        protected sealed override int DoWork(RequestSocket socket)
        {
            try
            {
                CallArgument argument = new CallArgument
                {
                    socket = socket
                };
                bool more;
                bool state = socket.TryReceiveFrameString(timeOut, out argument.client, out more);
                if (!state)
                {
                    return 0; //��ʱ
                }
                if (!more)
                {
                    return -1; //������
                }
                string empty;
                state = socket.TryReceiveFrameString(timeOut, out empty, out more);

                if (!state)
                {
                    return 0; //��ʱ
                }
                if (!more)
                {
                    return -1; //������
                }
                state = socket.TryReceiveFrameString(timeOut, out argument.callArg, out more);
                while (more)
                {
                    if (!state)
                    {
                        return 0; //��ʱ
                    }
                    string str;
                    state = socket.TryReceiveFrameString(timeOut, out str, out more);
                }
                OnClientCall(argument);
            }
            catch (Exception ex)
            {
                OnException(socket, ex);
                return -1;
            }
            return 1;
        }

        #endregion

        #region ��������
        /// <summary>
        /// ����������ʱ����
        /// </summary>
        /// <param name="arg"></param>
        protected void OnClientCall(object arg)
        {
            CallArgument argument = (CallArgument)arg;
            string result = OnCall(argument);
            if (RpcEnvironment.NetState != ZmqNetStatus.Runing)
            {
                LogRecorder.Warning($"����{argument.client}�������������ɣ������������ѹرգ��޷����ء�����Ϊ{argument.callArg}");
                return;
            }
            argument.socket.TrySendFrame(timeOut, argument.client, true);
            argument.socket.TrySendFrameEmpty(timeOut, true);
            argument.socket.TrySendFrame(timeOut, result);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="argument"></param>
        /// <returns></returns>
        protected virtual string OnCall(CallArgument argument)
        {
            return $"{{\"Result\":false,\"Id\":{Thread.CurrentThread.ManagedThreadId}}}";
        }

        #endregion
    }
}