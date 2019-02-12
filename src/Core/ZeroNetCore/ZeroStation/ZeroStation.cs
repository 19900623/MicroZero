using System;
using System.Threading;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core.ZeroManagemant;
using Agebull.ZeroNet.Core.ZeroServices.StateMachine;
using ZeroMQ;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// 站点
    /// </summary>
    public abstract class ZeroStation : IZeroObject
    {
        #region 基础信息

        /// <summary>
        /// 构造
        /// </summary>
        /// <param name="type">站点类型</param>
        /// <param name="isService">是否服务</param>
        protected ZeroStation(ZeroStationType type, bool isService)
        {
            StationType = type;
            IsService = isService;
            Name = GetType().Name;
            ConfigState = StationStateType.None;
            //Hearter = SystemManager.Instance;
        }
        /// <summary>
        /// 心跳器
        /// </summary>
        protected HeartManager Hearter => SystemManager.Instance;

        /// <summary>
        /// 是否服务
        /// </summary>
        public bool IsService { get; }

        /// <summary>
        /// 类型
        /// </summary>
        public ZeroStationType StationType { get; }
        /// <summary>
        /// 站点名称
        /// </summary>
        public string StationName { get; set; }

        /// <summary>
        /// 节点名称
        /// </summary>
        public string Name { get; protected internal set; }

        /// <summary>
        /// 实例名称
        /// </summary>
        public string RealName { get; protected set; }
        /// <summary>
        /// 实例名称
        /// </summary>
        public byte[] Identity { get; protected set; }

        #endregion

        #region 站点配置

        /// <summary>
        /// 站点配置
        /// </summary>
        public StationConfig Config { get; set; }

        /// <summary>
        /// 配置检查
        /// </summary>
        /// <returns></returns>
        protected virtual bool CheckConfig()
        {
            //取配置
            Config = ZeroApplication.Config[StationName] ?? OnNofindConfig();
            if (Config == null)
            {
                ZeroTrace.WriteError(StationName, "Station no find");
                RealState = StationState.ConfigError;
                ConfigState = StationStateType.ConfigError;
                return false;
            }
            //Config.OnStateChanged = OnConfigStateChanged;
            switch (Config.State)
            {
                case ZeroCenterState.None:
                case ZeroCenterState.Stop:
                    ZeroTrace.WriteError(StationName, "Station is stop");
                    RealState = StationState.Stop;
                    ConfigState = StationStateType.Stop;
                    return false;
                //case ZeroCenterState.Failed:
                //    ZeroTrace.WriteError(StationName, "Station is failed");
                //    RealState = StationState.ConfigError;
                //    ConfigState = ZeroCenterState.Stop;
                //    return false;
                case ZeroCenterState.Remove:
                    ZeroTrace.WriteError(StationName, "Station is remove");
                    RealState = StationState.Remove;
                    ConfigState = StationStateType.Remove;
                    return false;
                default:
                    return true;
            }

        }

        /// <summary>
        /// 初始化
        /// </summary>
        protected virtual StationConfig OnNofindConfig()
        {
            return null;
        }


        #endregion

        #region 状态管理


        /// <summary>
        ///     运行状态
        /// </summary>
        private int _realState;

        /// <summary>
        ///     运行状态
        /// </summary>
        public int RealState
        {
            get => _realState;
            set
            {
                if (_realState == value)
                    return;
                Interlocked.Exchange(ref _realState, value);
                ZeroTrace.SystemLog(StationName, nameof(RealState), StationState.Text(_realState));
            }
        }
//#if UseStateMachine
        /// <summary>
        /// 状态机
        /// </summary>
        protected IStationStateMachine StateMachine { get; private set; }
//#endif

        private StationStateType _configState;

        /// <summary>
        ///     配置状态
        /// </summary>
        public StationStateType ConfigState
        {
            get => _configState;
            set
            {
                if (_configState == value)
                    return;
//#if UseStateMachine
                switch (value)
                {
                    case StationStateType.None:
                    case StationStateType.Stop:
                    case StationStateType.Remove:
                        if (!(StateMachine is EmptyStateMachine /*&& !StateMachine.IsDisposed*/))
                            StateMachine = new EmptyStateMachine { Station = this };
                        break;
                    case StationStateType.Run:
                        if (ZeroApplication.CanDo)
                            StateMachine = new RunStateMachine { Station = this };
                        else
                            StateMachine = new StartStateMachine { Station = this };
                        break;
                    case StationStateType.Failed:
                        if (ZeroApplication.CanDo)
                            StateMachine = new FailedStateMachine { Station = this };
                        else
                            StateMachine = new StartStateMachine { Station = this };
                        break;
                    default:
                        StateMachine = new StartStateMachine { Station = this };
                        break;
                }
//#endif
                _configState = value;
//#if UseStateMachine
                ZeroTrace.SystemLog(StationName, nameof(ConfigState), value, StateMachine.GetTypeName());
//#else
//                ZeroTrace.SystemLog(StationName, nameof(ConfigState), value);
//#endif
            }
        }


        /// <summary>
        /// 取消标记
        /// </summary>
        protected CancellationTokenSource RunTaskCancel { get; set; }

        /// <summary>
        /// 能不能循环处理
        /// </summary>
        protected bool CanLoop => ZeroApplication.CanDo && ConfigState == StationStateType.Run && RunTaskCancel != null && !RunTaskCancel.IsCancellationRequested;


        #endregion

        #region 主流程

        /// <summary>
        /// 应用程序等待结果的信号量对象
        /// </summary>
        private readonly SemaphoreSlim _waitToken = new SemaphoreSlim(0, int.MaxValue);

        /// <summary>
        /// 初始化
        /// </summary>
        internal void Initialize()
        {
            if (Name == null)
                Name = StationName;
            RealState = StationState.Initialized;
            OnInitialize();
            ConfigState = StationName == null ? StationStateType.ConfigError : StationStateType.Initialized;
        }

        /// <summary>
        /// 开始
        /// </summary>
        /// <returns></returns>
        public bool Start()
        {
            while (_waitToken.CurrentCount > 0)
                _waitToken.Wait();
            if (ConfigState == StationStateType.None || ConfigState >= StationStateType.Stop || !ZeroApplication.CanDo)
                return false;
            ConfigState = StationStateType.Run;
            try
            {
                using (OnceScope.CreateScope(this))
                {
                    RealState = StationState.Start;
                    if (!CheckConfig())
                    {
                        ZeroApplication.OnObjectFailed(this);
                        return false;
                    }
                    //名称初始化
                    RealName = Config.StationName == ZeroApplication.Config.StationName
                        ? ZSocket.CreateRealName(false)
                        : ZSocket.CreateRealName(false, Config.StationName);
                    Identity = RealName.ToAsciiBytes();
                    ZeroTrace.SystemLog(StationName, Name, StationType, RealName, Config.WorkerCallAddress);
                    //扩展动作
                    if (!Prepare())
                    {
                        RealState = StationState.Failed;
                        ZeroApplication.OnObjectFailed(this);
                        return false;
                    }
                }
                //可执行
                //Hearter.HeartJoin(Config.StationName, RealName);
                //执行主任务
                RunTaskCancel = new CancellationTokenSource();
                new Thread(Run)
                {
                    IsBackground=true
                }.Start();
                _waitToken.Wait();
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
            }
            return true;
        }


        /// <summary>
        /// 命令轮询
        /// </summary>
        /// <returns></returns>
        private void Run()
        {
            bool success;
            try
            {
                using (OnceScope.CreateScope(this, LoopBegin, LoopComplete))
                {
                    success = Loop( /*RunTaskCancel.Token*/);
                }
            }
            catch (Exception e)
            {
                ZeroTrace.WriteException(StationName, e, "Run");
                RealState = StationState.Failed;
                success = false;
            }
            if (ConfigState < StationStateType.Stop)
                ConfigState = !ZeroApplication.CanDo || success ? StationStateType.Closed : StationStateType.Failed;
            GC.Collect();
//#if UseStateMachine
            StateMachine.End();
//#endif
        }


        /// <summary>
        /// 关闭
        /// </summary>
        /// <returns></returns>
        public bool Close()
        {
            if (RunTaskCancel == null || RunTaskCancel.IsCancellationRequested)
            {
                ZeroTrace.WriteError(StationName, "Close", "station not runing");
                return false;
            }
            SystemManager.Instance.HeartLeft(StationName, RealName);
            RealState = StationState.Closing;
            RunTaskCancel.Cancel();
            ZeroTrace.SystemLog(StationName, "Close", "Run is cancel,waiting... ");
            return true;
        }
        #endregion

        #region 扩展流程


        /// <summary>
        /// 初始化
        /// </summary>
        protected virtual void OnInitialize()
        {
        }

        /// <summary>
        /// 将要开始
        /// </summary>
        protected virtual bool Prepare()
        {
            return true;
        }


        /// <summary>
        /// 同步运行状态
        /// </summary>
        /// <returns></returns>
        private void LoopBegin()
        {
            lock (this)
            {
                RealState = StationState.BeginRun;
                if (ConfigState == StationStateType.Run)
                {
                    ZeroApplication.OnObjectActive(this);
                }
                else
                {
                    ZeroApplication.OnObjectFailed(this);
                }
                OnLoopBegin();
            }
            _waitToken.Release();
        }

        /// <summary>
        /// 轮询
        /// </summary>
        /// <returns>返回False表明需要重启</returns>
        protected abstract bool Loop(/*CancellationToken token*/);

        /// <summary>
        /// 同步关闭状态
        /// </summary>
        /// <returns></returns>
        private void LoopComplete()
        {
            lock (this)
            {
                if (RunTaskCancel == null)
                    return;
                OnLoopComplete();
                RunTaskCancel.Dispose();
                RunTaskCancel = null;
                RealState = StationState.Closed;
                ZeroApplication.OnObjectClose(this);
            }
            Hearter.HeartLeft(StationName, RealName);
        }

        /// <summary>
        /// 空转
        /// </summary>
        /// <returns></returns>
        protected virtual void OnLoopIdle()
        {
        }

        /// <summary>
        /// 开始执行的处理
        /// </summary>
        /// <returns></returns>
        protected virtual void OnLoopBegin()
        {
        }


        /// <summary>
        /// 关闭时的处理
        /// </summary>
        /// <returns></returns>
        protected virtual void OnLoopComplete()
        {
        }


        /// <summary>
        /// 析构
        /// </summary>
        protected virtual void DoDispose()
        {

        }
        /// <summary>
        /// 析构
        /// </summary>
        protected virtual void DoDestory()
        {

        }


        #endregion

        #region IZeroObject
        /// <summary>
        ///     要求心跳
        /// </summary>
        public virtual void OnHeartbeat()
        {
            //if (CanLoop)
            //    Hearter.Heartbeat(Config.StationName, RealName);
        }

        void IZeroObject.OnZeroInitialize()
        {
            Initialize();
        }

        bool IZeroObject.OnZeroStart()
        {
//#if UseStateMachine
            return StateMachine.Start();
//#else
//            if (RealState >= StationState.Start && RealState <= StationState.Closing)
//            {
//                return true;
//            }
//            return Start();
//#endif
        }

        /// <summary>
        /// 站点状态变更时调用
        /// </summary>
        void IZeroObject.OnStationStateChanged(StationConfig config)
        {
            while (RealState == StationState.Start || RealState == StationState.BeginRun || RealState == StationState.Closing)
                Thread.Sleep(10);
//#if UseStateMachine
            switch (config.State)
            {
                case ZeroCenterState.Initialize:
                case ZeroCenterState.Start:
                case ZeroCenterState.Run:
                case ZeroCenterState.Pause:
                    break;
                case ZeroCenterState.Failed:
                    if (!CanLoop)
                        break;
                    //运行状态中，与ZeroCenter一起重启
                    if (RealState == StationState.Run)
                        RealState = StationState.Closing;
                    ConfigState = StationStateType.Failed;
                    return;
                //以下状态如在运行，均可自动关闭
                case ZeroCenterState.Closing:
                case ZeroCenterState.Closed:
                case ZeroCenterState.Destroy:
                    if (RealState == StationState.Run)
                        RealState = StationState.Closing;
                    ConfigState = StationStateType.Closed;
                    return;
                case ZeroCenterState.NoFind:
                case ZeroCenterState.Remove:
                    if (RealState == StationState.Run)
                        RealState = StationState.Closing;
                    ConfigState = StationStateType.Remove;
                    return;
                default:
                    //case ZeroCenterState.None:
                    //case ZeroCenterState.Stop:
                    if (RealState == StationState.Run)
                        RealState = StationState.Closing;
                    ConfigState = StationStateType.Stop;
                    return;
            }
            if (!ZeroApplication.CanDo)
            {
                return;
            }
            //可启动状态检查是否未运行
            if (CanLoop)//这是一个可以正常运行的状态，无需要改变
                return;
            //是否未进行初始化
            if (ConfigState == StationStateType.None)
                Initialize();
            else
                ConfigState = StationStateType.Initialized;
            //同步启动它
            Start();
//#else
//            ConfigState = config.State;
//            if (RealState < StationState.Start || RealState > StationState.Closing)
//            {
//                if (config.State <= ZeroCenterState.Pause && ZeroApplication.CanDo)
//                {
//                    ZeroTrace.SystemLog(StationName, $"Start by config state({config.State}) changed");
//                    Start();
//                }
//            }
//            else
//            {
//                if (config.State >= ZeroCenterState.Failed || !ZeroApplication.CanDo)
//                {
//                    ZeroTrace.SystemLog(StationName, $"Close by config state({config.State}) changed");
//                    Close();
//                }
//            }
//#endif
        }

        bool IZeroObject.OnZeroEnd()
        {
//#if UseStateMachine
            return StateMachine.Close();
//#else
//            return Close();
//#endif
        }

        void IZeroObject.OnZeroDestory()
        {
            DoDestory();
        }

        private bool _isDisposed;

        void IDisposable.Dispose()
        {
            if (_isDisposed)
                return;
            _isDisposed = true;
            DoDispose();
        }
        #endregion
    }
}