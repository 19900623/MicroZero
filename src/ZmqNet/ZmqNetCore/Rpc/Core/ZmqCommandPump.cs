using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using Agebull.Common.Logging;
using NetMQ;
using Yizuan.Service.Api;

namespace Agebull.Zmq.Rpc
{
    /// <summary>
    ///     ZMQ����ִ�б�
    /// </summary>
    public abstract class ZmqCommandPump<TSocket> : IDisposable
        where TSocket : NetMQSocket
    {
        #region ��������
        /// <summary>
        /// ������
        /// </summary>
        public string Name { get; set; }
        /// <summary>
        /// ��������
        /// </summary>
        public void Run()
        {
            m_state = 3;
            DoRun();
            m_state = 4;
        }

        /// <summary>
        /// �߳�
        /// </summary>
        internal Thread thread;

        /// <summary>
        /// ����������
        /// </summary>
        protected virtual void DoRun()
        {
            if (thread != null)
                return;
            lock (this)
            {
                thread = new Thread(RunPump)
                {
                    Priority = ThreadPriority.BelowNormal,
                    IsBackground = true
                };
                thread.Start();
            }
        }

        /// <summary>
        /// ��������
        /// </summary>
        private int m_restart;

        /// <summary>
        /// �Ƿ�������
        /// </summary>
        public int RestartCount { get { return m_restart; } }
        /// <summary>
        /// ִ��״̬
        /// </summary>
        private int m_state;

        /// <summary>
        /// �Ƿ��ѳ�ʼ��
        /// </summary>
        public bool IsInitialized { get { return m_state > 0; } }


        /// <summary>
        /// �Ƿ�������
        /// </summary>
        public bool IsDisposed { get { return m_state > 4; } }

        /// <summary>
        /// ״̬ 0 ��ʼ״̬,1 ���ڳ�ʼ��,2 ��ɳ�ʼ��,3 ��ʼ����,4 ������,5 Ӧ�ùر� 6 �ѹر� 7 �������� 8 �������
        /// </summary>
        public int State { get { return m_state; } }

        public void SetClose()
        {
            m_state = 5;
        }

        /// <summary>
        ///     ��ʼ���ͻ���
        /// </summary>
        public void Initialize()
        {
            if (IsInitialized)
                return;
            m_state = 1;
            m_queue = new Queue<CommandArgument>();
            m_mutex = new Semaphore(0, ushort.MaxValue);
            DoInitialize();
            m_state = 2;
        }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        protected abstract void DoInitialize();

        /// <summary>
        ///     ����
        /// </summary>
        public void Dispose()
        {
            m_state = 7;
            if (m_mutex != null)
            {
                m_mutex.Dispose();
                m_mutex = null;
            }
            DoDispose();
            m_state = 8;
        }

        /// <summary>
        /// ����
        /// </summary>
        protected virtual void DoDispose()
        {

        }

        #endregion

        #region ��Ϣ����

        /// <summary>
        ///     C�˵�������ö���
        /// </summary>
        private Queue<CommandArgument> m_queue;

        /// <summary>
        ///     C��������ö�����
        /// </summary>
        private Semaphore m_mutex;

        /// <summary>
        ///     ȡ�ö�������
        /// </summary>
        protected CommandArgument Pop()
        {
            if (!m_mutex.WaitOne(1000))
            {
                return null;
            }
            CommandArgument mdMsg = null;
            if (m_queue.Count != 0)
            {
                mdMsg = m_queue.Dequeue();
            }
            return mdMsg;
        }

        /// <summary>
        ///     ����д�����
        /// </summary>
        /// <param name="cmdMsg"></param>
        protected void Push(CommandArgument cmdMsg)
        {
            m_queue.Enqueue(cmdMsg);
            m_mutex.Release();
        }

        #endregion

        #region ��������

        /// <summary>
        /// Ĭ�ϳ�ʱ����
        /// </summary>
        protected static readonly TimeSpan timeOut = new TimeSpan(0, 0, 10);

        /// <summary>
        /// ZMQ�����ַ
        /// </summary>
        public string ZmqAddress { get; set; }


        /// <summary>
        /// ����Socket����
        /// </summary>
        /// <param name="socket"></param>
        protected abstract void OptionSocktet(TSocket socket);

        /// <summary>
        /// ���Ӵ���
        /// </summary>
        /// <param name="socket"></param>
        protected virtual void OnConnected(TSocket socket)
        {

        }

        /// <summary>
        /// ���Ӵ���
        /// </summary>
        /// <param name="socket"></param>
        protected virtual void OnDeConnected(TSocket socket)
        {

        }


        /// <summary>
        /// ִ�й���
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="command">����</param>
        /// <param name="argument"></param>
        /// <param name="getResult">ȡ����ֵ��</param>
        /// <returns>����״̬</returns>
        protected string SendCommand(TSocket socket, string command, string argument, bool getResult)
        {
            try
            {
                bool state = socket.TrySendFrame(timeOut, command.ToAsciiBytes(), true);
                if (!state)
                {
                    Console.WriteLine("������ʧ��");
                    return "Failed";
                }
                state = socket.TrySendFrameEmpty(timeOut, true);
                if (!state)
                {
                    Console.WriteLine("������ʧ��");
                    return "Failed";
                }
                state = socket.TrySendFrame(timeOut, argument);
                if (!state)
                {
                    Console.WriteLine("������ʧ��");
                    return "Failed";
                }
                if (!getResult)
                    return "";
                bool more;
                string result = null;
                int retry = 0;
                do
                {
                    string re;
                    state = socket.TryReceiveFrameString(timeOut, out re, out more);
                    if (!state)
                    {
                        if (retry > 5)
                            break;
                        retry++;
                        more = true;
                        continue;
                    }
                    if (!string.IsNullOrWhiteSpace(re))
                        result = re;
                } while (more);
                return result ?? "Empty";
            }
            catch (Exception ex)
            {
                return OnException(socket, ex);
            }
        }


        /// <summary>
        /// ִ�й���
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="ex"></param>
        /// <returns>����״̬,����-1�ᵼ������</returns>
        protected string OnException(TSocket socket, Exception ex)
        {
            if (ex.Message == "Req.XSend - cannot send another request")
            {
                bool more;
                string re;
                do
                {
                    bool state = socket.TryReceiveFrameString(timeOut, out re, out more);
                    if (!state)
                        break;
                    Console.WriteLine(re);
                } while (more);
            }
            LogRecorder.Exception(ex);
            Trace.WriteLine(ex, GetType().Name + "DoWork");
            return "Exception";
        }

        //private NetMQMonitor monitor;
        /// <summary>
        /// ִ�й���
        /// </summary>
        /// <param name="socket"></param>
        /// <returns>����״̬,����-1�ᵼ������</returns>
        protected abstract int DoWork(TSocket socket);

        /// <summary>
        /// ����SOCKET����
        /// </summary>
        /// <remarks>�汾�����ݵ��¸ı�</remarks>
        protected abstract TSocket CreateSocket();

        /// <summary>
        ///     ִ�д����
        /// </summary>
        /// <returns></returns>
        protected void RunPump()
        {
            //�Ǽ��߳̿�ʼ
            RpcEnvironment.set_command_thread_start();
            //monitor?.Stop();
            int state = 0;
            Console.WriteLine($"���������({ZmqAddress})��������{this.GetType().Name}");
            using (var socket = CreateSocket())
            {
                //monitor = new NetMQMonitor(socket, $"inproc://pump_{Guid.NewGuid()}.rep", SocketEvents.All);
                OptionSocktet(socket);
                socket.Connect(ZmqAddress);
                OnConnected(socket);

                Console.WriteLine($"���������({ZmqAddress})������{this.GetType().Name}");

                while (m_state == 4 && RpcEnvironment.NetState == ZmqNetStatus.Runing)
                {
                    state = DoWork(socket);
                    if (state == -1)
                    {
                        Console.WriteLine($"���������({ZmqAddress})���ڴ���״̬{this.GetType().Name}");
                        break;
                    }
                }
                Console.WriteLine($"���������({ZmqAddress})���ڹر�{this.GetType().Name}");
                try
                {
                    OnDeConnected(socket);
                }
                catch (Exception e)
                {
                }
                try
                {
                    socket.Disconnect(ZmqAddress);
                }
                catch (Exception e)
                {
                }
            }
            Console.WriteLine($"���������({ZmqAddress})�ѹر�{this.GetType().Name}");
            //�Ǽ��̹߳ر�
            RpcEnvironment.set_command_thread_end();
            thread = null;
            OnTaskEnd(state);
        }

        /// <summary>
        /// ���������ʱ����(����ʵ��Ϊ�쳣ʱ����ִ��)
        /// </summary>
        /// <param name="state">״̬</param>
        protected virtual void OnTaskEnd(int state)
        {
            if (m_state == 5 || state != -1 || RpcEnvironment.NetState != ZmqNetStatus.Runing)
            {
                m_state = 6;
                return;
            }
            m_state = 6;
            m_restart++;
            DoRun();
        }
        #endregion
    }
}