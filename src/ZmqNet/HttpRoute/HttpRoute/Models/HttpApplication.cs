using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Microsoft.AspNetCore.Http;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Agebull.ZeroNet.LogRecorder;

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
            LogRecorder.Initialize(new RemoteRecorder());
            AppConfig.Initialize(Path.Combine(Startup.Configuration["contentRoot"], "route_config.json"));
            StationProgram.Run();
            RouteChahe.Flush();
            RuntimeWaring.Flush();
            RouteCommand.ZeroFlush();
            //Datas = new List<RouteData>();

        }
        #endregion


        #region ��������

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
        /// POST����
        /// </summary>
        /// <param name="context"></param>
        /// <returns></returns>
        public static Task Call(HttpContext context)
        {
            return Task.Factory.StartNew(CallTask, context);
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
                HttpProtocol.FormatResponse(context.Response);
                //����ҳת��
                if (uri.LocalPath.IndexOf(".", StringComparison.OrdinalIgnoreCase) > 0)
                {
                    context.Response.Redirect(AppConfig.Config.SystemConfig.ContextHost + uri.LocalPath.Trim('/'));
                    return;
                }
                //����֧��
                if (context.Request.Method.ToUpper() == "OPTIONS")
                {
                    HttpProtocol.Cros(context.Response);
                    return;
                }
                //����
                if (RouteCommand.InnerCommand(uri.LocalPath, context.Response))
                    return;
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                RuntimeWaring.Waring("Route", uri.LocalPath, e.Message);
                context.Response.WriteAsync(RouteRuntime.InnerError, Encoding.UTF8);
                return;
            }

            var router = new HttpRouter(context);
            
            HttpIoLog.OnBegin(router.Data);
            var counter = PerformanceCounter.OnBegin(router.Data);
            try
            {
                var checker = new SecurityChecker
                {
                    Request = context.Request
                };
                if (!checker.PreCheck())
                {
                    router.Data.Status = RouteStatus.DenyAccess;
                    context.Response.WriteAsync(RouteRuntime.Inner2Error, Encoding.UTF8);
                }
                else
                {
                    // ��������
                    router.Call();
                    LogRecorder.BeginStepMonitor("End");
                    // д�뷵��
                    router.WriteResult();
                    // ����
                    RouteChahe.CacheResult(router.Data);
                }
            }
            catch (Exception e)
            {
                router.Data.Status = RouteStatus.LocalError;
                LogRecorder.Exception(e);
                RuntimeWaring.Waring("Route", uri.LocalPath, e.Message);
                context.Response.WriteAsync(RouteRuntime.InnerError, Encoding.UTF8);
            }
            finally
            {
                //��ʱ
                counter.End(router.Data);
                HttpIoLog.OnEnd(router.Data);
            }
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