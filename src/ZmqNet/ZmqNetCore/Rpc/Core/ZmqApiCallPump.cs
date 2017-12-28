using System;
using System.Diagnostics;
using NetMQ;
using NetMQ.Sockets;

namespace Agebull.Zmq.Rpc
{
    /// <summary>
    ///     �ʴ�ʽ�����
    /// </summary>
    public class ZmqApiCallPump : ZmqCommandPump<RequestSocket>
    {
        #region ��Ϣ��

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
            socket.Options.Identity = RouteRpc.Singleton.Config.Token.ToAsciiBytes();
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
            CommandArgument callArg = Pop();
            if (callArg == null)
                return 0;
            RpcEnvironment.CacheCommand(callArg);
            try
            {
                bool state;
                //������������
                state = socket.TrySendFrame(timeOut, callArg.commandName);
                if (!state)
                    return -1;
                //���մ�����
                bool more;
                do
                {
                    string result;
                    state = socket.TryReceiveFrameString(timeOut, out result, out more);
                    if (!state)
                        return -1;
                    if (!string.IsNullOrWhiteSpace(result))
                        Console.WriteLine(result);
                } while (more);
            }
            catch (Exception ex)
            {
                OnException(socket, ex);
                callArg.cmdState = RpcEnvironment.NET_COMMAND_STATE_CLIENT_UNKNOW;
                return -1;
            }
            return 1;
        }

        #endregion

        #region ��������


        /// <summary>
        ///     �������
        /// </summary>
        /// <param name="command">�������</param>
        /// <param name="cacheCommand">�Ƿ񻺴�����(���ڻط�ʱ�ҵ�ԭʼ����)</param>
        public void request_net_cmmmand(CommandArgument command, bool cacheCommand = true)
        {
            if (command.cmdId == 0)
                command.cmdId = RpcEnvironment.NET_COMMAND_DATA_PUSH;
            Push(command);
        }

        /// <summary>
        ///     ���ͳ�������Դ���
        /// </summary>
        /// <param name="callArg"></param>
        /// <param name="state"></param>
        private void TryRequest(CommandArgument callArg, int state)
        {
            callArg.cmdState = state;
            callArg.OnRequestStateChanged(callArg);
            if (callArg.tryNum >= RpcEnvironment.NET_COMMAND_RETRY_MAX)
            {
                callArg.cmdState = RpcEnvironment.NET_COMMAND_STATE_RETRY_MAX;
                callArg.OnRequestStateChanged(callArg);
            }
            else if (callArg.tryNum >= 0)
            {
                callArg.tryNum++;
                Push(callArg);
            }
        }
        #endregion
    }
}