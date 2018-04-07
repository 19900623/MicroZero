using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using Agebull.Common.Logging;
using Agebull.ZeroNet.PubSub;
using Agebull.ZeroNet.ZeroApi;
using Newtonsoft.Json;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    ///     վ��Ӧ��
    /// </summary>
    public static class StationProgram
    {

        #region Station & Configs

        /// <summary>
        ///     վ�㼯��
        /// </summary>
        public static readonly Dictionary<string, StationConfig> Configs = new Dictionary<string, StationConfig>(StringComparer.OrdinalIgnoreCase);

        /// <summary>
        ///     վ�㼯��
        /// </summary>
        internal static readonly Dictionary<string, ZeroStation> Stations = new Dictionary<string, ZeroStation>(StringComparer.OrdinalIgnoreCase);

        /// <summary>
        ///     վ������
        /// </summary>
        private static LocalStationConfig _config;

        /// <summary>
        ///     վ������
        /// </summary>
        public static LocalStationConfig Config
        {
            get
            {
                if (_config != null)
                    return _config;
                if (!File.Exists("host.json"))
                    return _config = new LocalStationConfig();
                var json = File.ReadAllText("host.json");
                return _config = JsonConvert.DeserializeObject<LocalStationConfig>(json);
            }
            set => _config = value;
        }

        /// <summary>
        ///     ������Ĺ㲥��ַ
        /// </summary>
        public static string ZeroMonitorAddress => $"tcp://{Config.ZeroAddress}:{Config.ZeroMonitorPort}";

        /// <summary>
        ///     ������Ĺ����ַ
        /// </summary>
        public static string ZeroManageAddress => $"tcp://{Config.ZeroAddress}:{Config.ZeroManagePort}";

        /// <summary>
        /// </summary>
        /// <param name="station"></param>
        public static void RegisteStation(ZeroStation station)
        {
            if (Stations.ContainsKey(station.StationName))
            {
                Stations[station.StationName].Close();
                Stations[station.StationName] = station;
            }
            else
            {
                Stations.Add(station.StationName, station);
            }

            station.Config = GetConfig(station.StationName);
            if (State == StationState.Run)
                ZeroStation.Run(station);
        }

        #endregion

        #region System Command

        /// <summary>
        /// Զ�̵���
        /// </summary>
        /// <param name="station"></param>
        /// <param name="commmand"></param>
        /// <param name="argument"></param>
        /// <returns></returns>
        public static string Call(string station, string commmand, string argument)
        {
            var config = GetConfig(station);
            if (config == null)
            {
                return "{\"Result\":false,\"Message\":\"UnknowHost\",\"ErrorCode\":404}";
            }
            var result = config.OutAddress.RequestNet(commmand, ApiContext.RequestContext.RequestId, JsonConvert.SerializeObject(ApiContext.Current), argument);
            if (string.IsNullOrEmpty(result))
                return "{\"Result\":false,\"Message\":\"UnknowHost\",\"ErrorCode\":500}";
            if (result[0] == '{')
                return result;
            switch (result)
            {
                case "Invalid":
                    return "{\"Result\":false,\"Message\":\"��������\",\"ErrorCode\":-2}";
                case "NoWork":
                    return "{\"Result\":false,\"Message\":\"��������æ\",\"ErrorCode\":503}";
                default:
                    return result;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        public static void WriteLine(string message)
        {
            Console.WriteLine(message);
            Console.Write("$ > ");
        }
        /// <summary>
        ///     ִ�й�������
        /// </summary>
        /// <param name="commmand"></param>
        /// <param name="argument"></param>
        /// <returns></returns>
        public static bool Request(string commmand, string argument)
        {
            var result = ZeroManageAddress.RequestNet(commmand, argument);
            if (string.IsNullOrWhiteSpace(result))
            {
                WriteLine("*����ʱ");
                return false;
            }
            WriteLine(result);
            return true;
        }

        /// <summary>
        ///     ��ȡ����
        /// </summary>
        /// <returns></returns>
        public static StationConfig GetConfig(string stationName)
        {
            lock (Configs)
            {
                if (Configs.ContainsKey(stationName))
                    return Configs[stationName];
            }

            lock (Configs)
            {
                try
                {
                    var result = ZeroManageAddress.RequestNet("host", stationName);
                    if (result == null)
                    {
                        WriteLine("�޷���ȡ��Ϣ���ĵ�����");
                        return null;
                    }
                    var config = JsonConvert.DeserializeObject<StationConfig>(result);
                    Configs.Add(stationName, config);
                    return config;
                }
                catch (Exception e)
                {
                    LogRecorder.Exception(e);
                    return null;
                }
            }
        }

        /// <summary>
        ///     ��ȡ����
        /// </summary>
        /// <returns></returns>
        public static StationConfig InstallApiStation(string stationName)
        {
            StationConfig config;
            lock (Configs)
            {
                if (Configs.TryGetValue(stationName, out config))
                    return config;
            }

            lock (Configs)
            {
                try
                {
                    var result = ZeroManageAddress.RequestNet("install_api", stationName);
                    if (result == null)
                    {
                        WriteLine("�޷���ȡ��Ϣ���ĵ�����");
                        return null;
                    }
                    config = JsonConvert.DeserializeObject<StationConfig>(result);
                    Configs.Add(stationName, config);
                    return config;
                }
                catch (Exception e)
                {
                    LogRecorder.Exception(e);
                    return null;
                }
            }
        }

        #endregion

        #region Program Flow

        /// <summary>
        ///     ״̬
        /// </summary>
        public static StationState State { get; private set; }

        /// <summary>
        ///     ����
        /// </summary>
        public static void RunConsole()
        {
            Run();
            ConsoleInput();
        }

        private static void ConsoleInput()
        {
            Console.CancelKeyPress += Console_CancelKeyPress;
            while ( true)
            {
                var cmd = Console.ReadLine();
                if (string.IsNullOrWhiteSpace(cmd))
                    continue;
                switch (cmd.Trim().ToLower())
                {
                    case "quit":
                    case "exit":
                        Exit();
                        return;
                    case "stop":
                        Stop();
                        continue;
                    case "start":
                        Start();
                        continue;
                }
                var words = cmd.Split(' ', '\t', '\r', '\n');
                if (words.Length == 0)
                {
                    WriteLine("��������ȷ����");
                    continue;
                }
                Request(words[0], words.Length == 1 ? null : words[1]);
            }
        }

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            Exit();
        }

        /// <summary>
        ///     ��ʼ��
        /// </summary>
        public static void Initialize()
        {
            var discover = new ZeroStationDiscover();
            discover.FindApies(Assembly.GetCallingAssembly());
            if (discover.ApiItems.Count == 0)
                return;
            var station = new ApiStation
            {
                Config = GetConfig(Config.StationName),
                StationName = Config.StationName
            };
            foreach (var action in discover.ApiItems)
            {
                if (action.Value.HaseArgument)
                    station.RegistAction(action.Key, action.Value.ArgumentAction, action.Value.AccessOption >= ApiAccessOption.Customer);
                else
                    station.RegistAction(action.Key, action.Value.Action, action.Value.AccessOption >= ApiAccessOption.Customer);
            }
            Stations.Add(Config.StationName, station);
        }

        /// <summary>
        ///     ֹͣ
        /// </summary>
        public static void Start()
        {
            switch (State)
            {
                case StationState.Run:
                    WriteLine("*run...");
                    return;
                case StationState.Closing:
                    WriteLine("*closing...");
                    return;
                case StationState.Destroy:
                    WriteLine("*destroy...");
                    return;
            }
            Run();
        }

        /// <summary>
        ///     ֹͣ
        /// </summary>
        public static void Stop()
        {
            WriteLine("Program Stop.");
            State = StationState.Closing;
            foreach (var stat in Stations)
                stat.Value.Close();
            while (Stations.Values.Any(p => p.RunState == StationState.Run))
            {
                Console.Write(".");
                Thread.Sleep(100);
            }
            ZeroPublisher.Stop();
            State = StationState.Closed;
            WriteLine("@");
        }

        /// <summary>
        ///     �ر�
        /// </summary>
        public static void Exit()
        {
            if (State == StationState.Run)
                Stop();
            State = StationState.Destroy;
            WriteLine("Program Exit");
            Process.GetCurrentProcess().Close();
        }

        /// <summary>
        ///     ����ϵͳ����
        /// </summary>
        public static void Run()
        {
            State = StationState.Run;
            ZeroPublisher.Start();
            foreach (var station in Stations.Values)
                ZeroStation.Run(station);
            Task.Factory.StartNew(SystemMonitor.RunMonitor);
        }

        #endregion
    }
}