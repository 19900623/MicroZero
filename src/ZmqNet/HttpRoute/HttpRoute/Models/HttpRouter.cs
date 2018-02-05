using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using Agebull.Common.Base;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.ZeroApi;
using Gboxt.Common.DataModel;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Microsoft.AspNetCore.Http;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    ///     ����ӳ�������
    /// </summary>
    internal class HttpRouter : ScopeBase
    {

        #region ����

        /// <summary>
        ///     Http������
        /// </summary>
        public HttpContext HttpContext { get; }

        /// <summary>
        ///     Http����
        /// </summary>
        public HttpRequest Request { get; }

        /// <summary>
        ///     Http����
        /// </summary>
        public HttpResponse Response { get; }

        /// <summary>
        ///     Http����
        /// </summary>
        public RouteData Data { get; }
        
        #endregion

        #region ����

        /// <summary>
        ///     �ڲ�����
        /// </summary>
        /// <param name="context"></param>
        internal HttpRouter(HttpContext context)
        {
            Data = new RouteData();
            HttpContext = context;
            Request = context.Request;
            Response = context.Response;
        }
        

        protected override void OnDispose()
        {
        }

        /// <summary>
        /// ����
        /// </summary>
        internal void Call()
        {
            Data.Uri = Request.GetUri();
            Data.HttpMethod = Request.Method.ToUpper();

            if (!CheckCall())
                return;
            // 1 ��ȫ���
            if (!SecurityCheck())
                return;
            // 2 ��ʼ��·����Ϣ
            if (!InitializeContext())
                return;
            // 3 ������ٴ���
            if (RouteRuntime.LoadCache(Data.Uri, Data.Bearer, out Data.CacheSetting, out Data.CacheKey, ref Data.ResultMessage))
            {
                //�ҵ������ػ���
                Data.Status = RouteStatus.Cache;
                return;
            }
            // 4 Զ�̵���
            Data.ResultMessage = Data.RouteHost.ByZero ? CallZero() : CallHttp();
            // 5 ������
            Data.IsSucceed = RouteRuntime.CheckResult(Data);
        }
        /// <summary>
        /// ����������
        /// </summary>
        /// <returns></returns>
        private bool CheckCall()
        {
            var words = Data.Uri.LocalPath.Split('/', 2, StringSplitOptions.RemoveEmptyEntries);
            if (words.Length <= 1)
            {
                Data.Status = RouteStatus.FormalError;
                Data.ResultMessage = RouteRuntime.DenyAccess;
                return false;
            }
            Data.HostName = words[0];
            Data.ApiName = words[1];
            return true;
        }
        /// <summary>
        ///     ��ȫ���
        /// </summary>
        private bool SecurityCheck()
        {
            string authorization = Request.Headers["Authorization"];
            if (string.IsNullOrWhiteSpace(authorization))
            {
                Data.Bearer = Request.Query["ClientKey"];
                return true;
            }

            var words = authorization.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
            if (words.Length != 2 || !string.Equals(words[0], "Bearer", StringComparison.OrdinalIgnoreCase) || words[1].Equals("null") || words[1].Equals("undefined"))
                Data.Bearer = null;
            else
                Data.Bearer = words[1];

            var checker = new SecurityChecker
            {
                Request = Request,
                Bearer = Data.Bearer
            };
            if (checker.Check())
                return true;
            Data.Status = RouteStatus.DenyAccess;
            Data.ResultMessage = AppConfig.Config.SystemConfig.BlockHost;
            Response.Redirect(AppConfig.Config.SystemConfig.BlockHost, false);
            Data.Redirect = true;
            return false;
        }


        /// <summary>
        ///     д�뷵��
        /// </summary>
        internal void WriteResult()
        {
            if (Data.Redirect)
                return;
            Response.Headers.Add("Access-Control-Allow-Origin", "*");
            Response.WriteAsync(Data.ResultMessage ?? RouteRuntime.RemoteEmptyError, Encoding.UTF8);
        }

        #endregion

        #region ·��

        /// <summary>
        ///     ��ʼ������������
        /// </summary>
        private bool InitializeContext()
        {

            ApiContext.Current.Request.Bear = Data.Bearer;
            ApiContext.Current.Request.RequestId = RandomOperate.Generate(8);
            ApiContext.Current.Request.Ip = HttpContext.Connection.RemoteIpAddress.ToString();
            ApiContext.Current.Request.Port = HttpContext.Connection.RemotePort.ToString();
            ApiContext.Current.Request.ServiceKey = ApiContext.MyServiceKey;
            ApiContext.Current.Request.ArgumentType = ArgumentType.Json;
            ApiContext.Current.Request.UserAgent = Request.Headers["User-Agent"];

            if (!AppConfig.Config.RouteMap.TryGetValue(Data.HostName, out Data.RouteHost))
                Data.RouteHost = HostConfig.DefaultHost;
            return true;
        }

        #endregion

        #region Http

        /// <summary>
        ///     Զ�̵���
        /// </summary>
        /// <returns></returns>
        private string CallHttp()
        {
            // ��ǰ������õ�ģ�Ͷ�Ӧ����������
            string httpHost;

            // ��ǰ������õ�Api����
            string httpApi = Data.RouteHost == HostConfig.DefaultHost ? Data.Uri.PathAndQuery : $"{Data.ApiName}{Data.Uri.Query}";

            // ��������
            if (Data.RouteHost.Hosts.Length == 1)
            {
                httpHost = Data.RouteHost.Hosts[0];
            }
            else lock (Data.RouteHost)
                {
                    //ƽ������
                    httpHost = Data.RouteHost.Hosts[Data.RouteHost.Next];
                    if (++Data.RouteHost.Next >= Data.RouteHost.Hosts.Length)
                        Data.RouteHost.Next = 0;
                }
            // Զ�̵���
            var caller = new HttpApiCaller(httpHost)
            {
                Bearer = $"Bearer {ApiContext.RequestContext.Bear}"
            };
            var req = caller.CreateRequest(httpApi, Data.HttpMethod, Request, Data);

            LogRecorder.BeginStepMonitor("�ڲ�HTTP����");
            LogRecorder.MonitorTrace($"Url:{req.RequestUri.PathAndQuery}");
            LogRecorder.MonitorTrace($"Auth:{caller.Bearer}");

            try
            {
                // Զ�̵���״̬
                Data.ResultMessage = caller.GetResult(req, out var _webStatus);
                LogRecorder.MonitorTrace(_webStatus.ToString());
                if (_webStatus != WebExceptionStatus.Success)
                    Data.Status = RouteStatus.RemoteError;
            }
            catch (Exception ex)
            {
                LogRecorder.Exception(ex);
                LogRecorder.MonitorTrace($"�����쳣��{ex.Message}");
                Data.ResultMessage = RouteRuntime.NetworkError;
                Data.Status = RouteStatus.RemoteError;
            }
            finally
            {
                LogRecorder.MonitorTrace(Data.ResultMessage);
                LogRecorder.EndStepMonitor();
            }
            return Data.ResultMessage;
        }


        #endregion

        #region Zero

        /// <summary>
        ///     Զ�̵���
        /// </summary>
        /// <returns></returns>
        private string CallZero()
        {
            var values = new Dictionary<string, string>();
            //��������
            foreach (var query in Request.Query.Keys)
                if (!values.ContainsKey(query))
                    values.Add(query, Request.Query[query]);

            if (Data.HttpMethod == "POST")
                if (Request.ContentLength > 0)
                    foreach (var form in Request.Form.Keys)
                        if (!values.ContainsKey(form))
                            values.Add(form, Request.Form[form]);

            LogRecorder.BeginStepMonitor("�ڲ�Zero����");
            LogRecorder.MonitorTrace($"Station:{Data.HostName}");
            LogRecorder.MonitorTrace($"Command:{Data.ApiName}");

            // Զ�̵���״̬
            try
            {
                Data.ResultMessage = StationProgram.Call(Data.HostName, Data.ApiName, JsonConvert.SerializeObject(values));
            }
            catch (Exception ex)
            {
                LogRecorder.Exception(ex);
                LogRecorder.MonitorTrace($"�����쳣��{ex.Message}");
                Data.ResultMessage = RouteRuntime.NetworkError;
                Data.Status = RouteStatus.RemoteError;
            }
            finally
            {
                LogRecorder.MonitorTrace(Data.ResultMessage);
                LogRecorder.EndStepMonitor();
            }

            return Data.ResultMessage;
        }

        #endregion
    }
}