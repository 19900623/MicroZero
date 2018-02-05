using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Microsoft.AspNetCore.Http;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ����ӳ�������
    /// </summary>
    public class HttpApplication
    {
        #region ��ʼ��

        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void Initialize()
        {
            // ��־֧��
            //Agebull.Common.Logging.LogRecorder.GetRequestIdFunc = () => ApiContext.RequestContext.RequestId;.
            LogRecorder.Initialize();
            AppConfig.Initialize();
            StationProgram.Run();
            RouteRuntime.Flush();
            ZeroFlush();
            //Datas = new List<RouteData>();

        }
        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void Flush()
        {
            AppConfig.Initialize();
            RouteRuntime.Flush();
            ZeroFlush();
            //Datas = new List<RouteData>();

        }

        #endregion

        #region ZeroNet


        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void ZeroFlush()
        {
            if (!AppConfig.Config.SystemConfig.FireZero)
                return;

            StationProgram.StationEvent -= StationProgram_StationEvent;

            foreach (var host in AppConfig.Config.RouteMap.Where(p => p.Value.ByZero).ToArray())
                AppConfig.Config.RouteMap.Remove(host.Key);

            foreach (var station in StationProgram.Configs.Values.Where(p => p.StationType == 2))
            {
                var host = new HostConfig
                {
                    ByZero = true
                };
                if (!AppConfig.Config.RouteMap.ContainsKey(station.StationName))
                    AppConfig.Config.RouteMap.Add(station.StationName, host);
                foreach (var name in station.StationAlias)
                    if (!AppConfig.Config.RouteMap.ContainsKey(name))
                        AppConfig.Config.RouteMap.Add(name, host);
            }
            StationProgram.StationEvent += StationProgram_StationEvent;
        }
        private static void StationProgram_StationEvent(object sender, StationProgram.StationEventArgument e)
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
                    var route = new HostConfig
                    {
                        ByZero = true
                    };
                    if (!AppConfig.Config.RouteMap.ContainsKey(e.EventConfig.StationName))
                        AppConfig.Config.RouteMap.Add(e.EventConfig.StationName, route);
                    foreach (var name in e.EventConfig.StationAlias)
                        if (!AppConfig.Config.RouteMap.ContainsKey(name))
                            AppConfig.Config.RouteMap.Add(name, route);
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

        #region ��������

        /// <summary>
        /// POST����
        /// </summary>
        /// <param name="h"></param>
        /// <returns></returns>
        public static Task Call(HttpContext h)
        {
            return Task.Factory.StartNew(CallTask, h);
        }

        /// <summary>
        /// ����
        /// </summary>
        /// <param name="arg"></param>
        /// <returns></returns>
        public static void CallTask(object arg)
        {
            CallTask((HttpContext)arg);
        }

        /// <summary>
        /// ����
        /// </summary>
        /// <param name="context"></param>
        /// <returns></returns>
        public static void CallTask(HttpContext context)
        {
            var uri = context.Request.GetUri();
            try
            {
                context.Response.ContentType = "text/plain; charset=utf-8";
                context.Response.Headers["Content-Type"] = "text/plain; charset=utf-8";
                //����ҳת��
                if (uri.LocalPath.IndexOf(".", StringComparison.OrdinalIgnoreCase) > 0)
                {
                    context.Response.Redirect(AppConfig.Config.SystemConfig.ContextHost + uri.LocalPath.Trim('/'));
                    return;
                }
                //����֧��
                if (context.Request.Method.ToUpper() == "OPTIONS")
                {
                    Cros(context.Response);
                    return;
                }
                //����
                switch (uri.LocalPath)
                {
                    case "/":
                        context.Response.WriteAsync("Wecome ZeroNet Http Router!", Encoding.UTF8);
                        return;
                    case "/_1_clear_1_":
                        Flush();
                        context.Response.WriteAsync(JsonConvert.SerializeObject(AppConfig.Config, Formatting.Indented), Encoding.UTF8);
                        return;
                    case "/_1_counter_1_/info":
                        context.Response.WriteAsync(JsonConvert.SerializeObject(RouteCounter.Station, Formatting.Indented), Encoding.UTF8);
                        return;
                    case "/_1_counter_1_/save":
                        RouteCounter.Save();
                        context.Response.WriteAsync(JsonConvert.SerializeObject(RouteCounter.Station, Formatting.Indented), Encoding.UTF8);
                        return;
                    case "/_1_config_1_":
                        context.Response.WriteAsync(JsonConvert.SerializeObject(AppConfig.Config, Formatting.Indented), Encoding.UTF8);
                        return;
                    case "/_1_warings_1_":
                        context.Response.WriteAsync(JsonConvert.SerializeObject(RouteRuntime.WaringsTime, Formatting.Indented), Encoding.UTF8);
                        return;
                    case "/_1_cache_1_":
                        context.Response.WriteAsync(JsonConvert.SerializeObject(RouteRuntime.Cache, Formatting.Indented), Encoding.UTF8);
                        return;
                }
                
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                RouteRuntime.RuntimeWaring("Route", uri.LocalPath, e.Message);
                context.Response.WriteAsync(RouteRuntime.InnerError, Encoding.UTF8);
                return;
            }

            var router = new HttpRouter(context);
            HttpIoLog.OnBegin(context.Request, router.Data);
            try
            {
                var counter = RouteCounter.Begin();
                // ��������
                router.Call();
                LogRecorder.BeginStepMonitor("End");
                // д�뷵��
                router.WriteResult();
                // ����
                RouteRuntime.CacheResult(router.Data);
                //��ʱ
                counter.End(router.Data);
            }
            catch (Exception e)
            {
                router.Data.Status = RouteStatus.LocalError;
                LogRecorder.Exception(e);
                RouteRuntime.RuntimeWaring("Route", uri.LocalPath, e.Message);
                context.Response.WriteAsync(RouteRuntime.InnerError, Encoding.UTF8);
            }
            finally
            {
                HttpIoLog.OnEnd(router.Data.Status, router.Data.ResultMessage);
            }
        }
        #endregion
        #region ����֧��

        /// <summary>
        ///     ����֧��
        /// </summary>
        private static void Cros(HttpResponse response)
        {
            response.Headers.Add("Access-Control-Allow-Methods", new[] { "GET", "POST" });
            response.Headers.Add("Access-Control-Allow-Headers",
                new[] { "x-requested-with", "content-type", "authorization", "*" });
            response.Headers.Add("Access-Control-Allow-Origin", "*");
        }

        #endregion

        #region ���ú���
        /*
        /// <summary>
        /// ����
        /// </summary>
        internal static List<RouteData> Datas = new List<RouteData>();

        public static Mutex Mutex = new Mutex();

        /// <summary>
        /// ���̽�����Ĵ���
        /// </summary>
        internal static void PushFlowExtend(RouteData data)
        {
            Datas.Add(data);
            Mutex.ReleaseMutex();
        }

        /// <summary>
        /// ���̽�����Ĵ���
        /// </summary>
        internal static void FlowExtendTask()
        {
            while (true)
            {
                if (!Mutex.WaitOne(1000))
                {
                    continue;
                }
                var datas = Datas;
                Datas = new List<RouteData>();
                Mutex.ReleaseMutex();
                //��ദ���50��
                var index = 0;
                if (datas.Count > 50)
                {
                    index = datas.Count - 50;
                }
                for (; index < datas.Count; index++)
                {
                    RouteRuntime.CacheData(datas[index]);
                }
            }
        }*/

        #endregion
    }
}