using System;
using System.Threading.Tasks;
using Agebull.Common.Logging;
using NetMQ;
using NetMQ.Sockets;
using Newtonsoft.Json;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// ϵͳ������
    /// </summary>
    public class SystemMonitor
    {
        /// <summary>
        ///     ����ϵͳ����
        /// </summary>
        public static void RunMonitor()
        {
            var timeout = new TimeSpan(0, 0, 1);
            try
            {
                StationProgram.WriteLine("System Monitor Runing...");
                var subscriber = new SubscriberSocket();
                subscriber.Options.Identity = StationProgram.Config.StationName.ToAsciiBytes();
                subscriber.Options.ReconnectInterval = new TimeSpan(0, 0, 0, 0, 200);
                subscriber.Connect(StationProgram.ZeroMonitorAddress);
                subscriber.Subscribe("");

                while (StationProgram.State == StationState.Run)
                {
                    if (!subscriber.TryReceiveFrameString(timeout, out var result))
                        continue;
                    OnMessagePush(result);
                }
            }
            catch (Exception e)
            {
                StationProgram.WriteLine(e.Message);
                LogRecorder.Exception(e);
            }
            if (StationProgram.State == StationState.Run)
                Task.Factory.StartNew(RunMonitor);
        }

        /// <summary>
        ///     �յ���Ϣ�Ĵ���
        /// </summary>
        /// <param name="msg"></param>
        public static void OnMessagePush(string msg)
        {
            if (String.IsNullOrEmpty(msg))
                return;
            var array = msg.Split(new[] { ' ' }, 3);
            var cmd = array[0];
            var station = array.Length > 1 ? array[1] : "*";
            var content = array.Length > 2 ? array[2] : "{}";
            switch (cmd)
            {
                case "system_start":
                    system_start(content);
                    break;
                case "system_stop":
                    system_stop(content);
                    break;
                case "station_join":
                    station_join(station, content);
                    break;
                case "station_left":
                    station_left(station);
                    break;
                case "station_pause":
                    station_pause(station, content);
                    break;
                case "station_resume":
                    station_resume(station, content);
                    break;
                case "station_closing":
                    station_closing(station, content);
                    break;
                case "station_install":
                    station_install(station, content);
                    break;
                case "worker_heat":
                    station_heat(station, content);
                    break;
            }
        }

        /// <summary>
        /// վ���¼�����
        /// </summary>
        public class StationEventArgument : EventArgs
        {
            /// <summary>
            /// ����
            /// </summary>
            /// <param name="eventName"></param>
            /// <param name="config"></param>
            public StationEventArgument(string eventName, StationConfig config)
            {
                EventConfig = config;
                EventName = eventName;
            }
            /// <summary>
            /// վ������
            /// </summary>
            public string EventName { get; }
            /// <summary>
            /// ����
            /// </summary>
            public StationConfig EventConfig { get; }
        }

        /// <summary>
        /// վ���¼�����
        /// </summary>
        public static event EventHandler<StationEventArgument> StationEvent;

        /// <summary>
        /// վ������
        /// </summary>
        /// <param name="station"></param>
        /// <param name="content"></param>
        private static void station_heat(string station, string content)
        {
            if (String.IsNullOrEmpty(content))
                return;
            StationConfig cfg;
            try
            {
                cfg = JsonConvert.DeserializeObject<StationConfig>(content);
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return;
            }
            cfg.State = StationState.None;
            if (StationProgram.Configs.ContainsKey(station))
                StationProgram.Configs[station] = cfg;
            else
                StationProgram.Configs.Add(station, cfg);

            StationEvent?.Invoke(cfg, new StationEventArgument("station_heat", cfg));
        }

        private static void station_install(string station, string content)
        {
            if (String.IsNullOrEmpty(content))
                return;
            StationConfig cfg;
            try
            {
                cfg = JsonConvert.DeserializeObject<StationConfig>(content);
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return;
            }
            cfg.State = StationState.None;
            if (StationProgram.Configs.ContainsKey(station))
                StationProgram.Configs[station] = cfg;
            else
                StationProgram.Configs.Add(station, cfg);
            StationEvent?.Invoke(cfg, new StationEventArgument("station_install", cfg));
        }

        private static void station_closing(string station, string content)
        {
            if (StationProgram.Configs.TryGetValue(station, out var cfg))
                cfg.State = StationState.Closing;
            if (StationProgram.Stations.ContainsKey(station))
            {
                StationProgram.WriteLine($"{station} is close");
                StationProgram.Stations[station].Close();
            }

            StationEvent?.Invoke(cfg, new StationEventArgument("station_closing", cfg));
        }

        private static void station_resume(string station, string content)
        {
            if (StationProgram.Configs.TryGetValue(station, out var cfg))
                cfg.State = StationState.Run;
            if (StationProgram.Stations.ContainsKey(station))
            {
                StationProgram.WriteLine($"{station} is resume");
                ZeroStation.Run(StationProgram.Stations[station]);
            }

            StationEvent?.Invoke(cfg, new StationEventArgument("station_resume", cfg));
        }

        private static void station_pause(string station, string content)
        {
            if (StationProgram.Configs.TryGetValue(station, out var cfg))
                cfg.State = StationState.Pause;
            if (StationProgram.Stations.ContainsKey(station))
            {
                StationProgram.WriteLine($"{station} is pause");
                StationProgram.Stations[station].Close();
            }

            StationEvent?.Invoke(cfg, new StationEventArgument("station_pause", cfg));
        }

        private static void station_left(string station)
        {
            if (StationProgram.Configs.TryGetValue(station, out var cfg))
                cfg.State = StationState.Closed;
            if (StationProgram.Stations.ContainsKey(station))
            {
                StationProgram.WriteLine($"{station} is left");
                StationProgram.Stations[station].Close();
            }

            StationEvent?.Invoke(cfg, new StationEventArgument("station_left", cfg));
        }

        private static void station_join(string station, string content)
        {
            if (String.IsNullOrEmpty(content))
                return;
            StationConfig cfg;
            try
            {
                cfg = JsonConvert.DeserializeObject<StationConfig>(content);
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return;
            }
            cfg.State = StationState.Run;
            if (StationProgram.Configs.ContainsKey(station))
                StationProgram.Configs[station] = cfg;
            else
                StationProgram.Configs.Add(station, cfg);
            if (StationProgram.Stations.ContainsKey(station))
            {
                StationProgram.Stations[station].Config = cfg;
                StationProgram.WriteLine($"{station} is join");
                StationEvent?.Invoke(cfg, new StationEventArgument("station_join", cfg));
                ZeroStation.Run(StationProgram.Stations[station]);
            }
        }

        private static void system_stop(string content)
        {
            StationEvent?.Invoke(null, new StationEventArgument("system_stop", null));
            StationProgram.WriteLine(content);
            foreach (var sta in StationProgram.Stations.Values)
            {
                StationProgram.WriteLine($"Close {sta.StationName}");
                sta.Close();
                StationEvent?.Invoke(sta, new StationEventArgument("station_closing", sta.Config));
            }
            StationProgram.Configs.Clear();
        }

        private static void system_start(string content)
        {
            StationProgram.WriteLine(content);
            StationProgram.Configs.Clear();

            StationEvent?.Invoke(null, new StationEventArgument("system_start", null));
            foreach (var sta in StationProgram.Stations.Values)
            {
                StationProgram.WriteLine($"Restart {sta.StationName}");
                ZeroStation.Run(sta);
                StationEvent?.Invoke(sta, new StationEventArgument("station_join", sta.Config));
            }
        }
    }
}