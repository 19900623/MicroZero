using System.Linq;
using System.Text;
using Agebull.ZeroNet.Core;
using Microsoft.AspNetCore.Http;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// �ڲ�����
    /// </summary>
    public class RouteCommand
    {
        /// <summary>
        /// �ڲ������
        /// </summary>
        /// <param name="url"></param>
        /// <param name="response"></param>
        /// <returns></returns>
        public static bool InnerCommand(string url, HttpResponse response)
        {

            //����
            switch (url)
            {
                case "/":
                    response.WriteAsync("Wecome ZeroNet Http Router!", Encoding.UTF8);
                    return true;
                case "/_1_clear_1_":
                    Flush();
                    response.WriteAsync(JsonConvert.SerializeObject(AppConfig.Config, Formatting.Indented), Encoding.UTF8);
                    return true;
                //case "/_1_counter_1_/info":
                //    response.WriteAsync(JsonConvert.SerializeObject(RouteCounter.Station, Formatting.Indented), Encoding.UTF8);
                //    return true;
                //case "/_1_counter_1_/save":
                //    RouteCounter.Save();
                //    response.WriteAsync(JsonConvert.SerializeObject(RouteCounter.Station, Formatting.Indented), Encoding.UTF8);
                //    return true;
                case "/_1_config_1_":
                    response.WriteAsync(JsonConvert.SerializeObject(AppConfig.Config, Formatting.Indented), Encoding.UTF8);
                    return true;
                    //case "/_1_warings_1_":
                    //    response.WriteAsync(JsonConvert.SerializeObject(RuntimeWaring.WaringsTime, Formatting.Indented), Encoding.UTF8);
                    //    return true;
                    //case "/_1_cache_1_":
                    //    response.WriteAsync(JsonConvert.SerializeObject(RouteChahe.Cache, Formatting.Indented), Encoding.UTF8);
                    //    return true;
            }

            return false;
        }


        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void Flush()
        {
            AppConfig.Initialize();
            RouteChahe.Flush();
            RuntimeWaring.Flush();
            ZeroFlush();
            //Datas = new List<RouteData>();

        }

        #region ZeroNet


        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void ZeroFlush()
        {
            if (!AppConfig.Config.SystemConfig.FireZero)
                return;

            SystemMonitor.StationEvent -= StationProgram_StationEvent;

            foreach (var host in AppConfig.Config.RouteMap.Where(p => p.Value.ByZero).ToArray())
                AppConfig.Config.RouteMap.Remove(host.Key);

            foreach (var station in StationProgram.Configs.Values.Where(p => p.StationType == ZeroStation.StationTypeApi))
            {
                ApiStationJoin(station);
            }
            SystemMonitor.StationEvent += StationProgram_StationEvent;
        }

        static void ApiStationJoin(StationConfig station)
        {
            StationProgram.WriteInfo($"Zero Station:{station.StationName}");
            var host = new HostConfig
            {
                ByZero = true
            };
            if (!AppConfig.Config.RouteMap.ContainsKey(station.StationName))
                AppConfig.Config.RouteMap.Add(station.StationName, host);
            else
                AppConfig.Config.RouteMap[station.StationName] = host;
            if (station.StationAlias == null)
                return;
            foreach (var name in station.StationAlias)
                if (!AppConfig.Config.RouteMap.ContainsKey(name))
                    AppConfig.Config.RouteMap.Add(name, host);
        }

        private static void StationProgram_StationEvent(object sender, SystemMonitor.StationEventArgument e)
        {
            switch (e.EventName)
            {
                case "system_start":
                    break;
                case "system_stop":
                    foreach (var host in AppConfig.Config.RouteMap.Where(p => p.Value.ByZero).ToArray())
                        AppConfig.Config.RouteMap.Remove(host.Key);
                    break;
                case "worker_heat":
                case "station_resume":
                case "station_install":
                case "station_join":
                    ApiStationJoin(e.EventConfig);
                    break;
                case "station_left":
                    break;
                case "station_pause":
                case "station_closing":
                    AppConfig.Config.RouteMap.Remove(e.EventConfig.StationName);
                    foreach (var name in e.EventConfig.StationAlias)
                        AppConfig.Config.RouteMap.Remove(name);
                    break;
            }
        }

        #endregion
    }
}