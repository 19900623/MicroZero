using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Yizuan.Service.Api;

namespace ExternalStation.Models
{
    /// <summary>
    /// ����ӳ�������
    /// </summary>
    public class HttpApplication
    {
        /// <summary>
        /// ��ʼ��
        /// </summary>
        public static void Initialize()
        {
            // ��־֧��
            Agebull.Common.Logging.LogRecorder.GetRequestIdFunc = () => ApiContext.RequestContext.RequestId;
            Agebull.Common.Logging.LogRecorder.Initialize();
            RouteData.Flush();
        }
        /// <summary>
        /// POST����
        /// </summary>
        /// <param name="h"></param>
        /// <returns></returns>
        public static Task Call(HttpContext h)
        {
            return Task.Factory.StartNew(Call, h);
        }

        /// <summary>
        /// ����
        /// </summary>
        /// <param name="arg"></param>
        /// <returns></returns>
        public static void Call(object arg)
        {
            var start = DateTime.Now;
            HttpRouter.Todo((HttpContext)arg);
            var sm = DateTime.Now - start;
            //if (sm.TotalMilliseconds > 200)
            //    LogRecorder.Warning($"ִ��ʱ���쳣({sm.TotalMilliseconds}):{url.LocalPath}");
            //else
                Console.WriteLine($"{((HttpContext)arg).Connection.LocalIpAddress}:{sm.TotalMilliseconds}");
        }
    }
}