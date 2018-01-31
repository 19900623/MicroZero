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
    /// ·������
    /// </summary>
    internal class RouteData
    {
        /// <summary>
        /// �����ַ
        /// </summary>
        public Uri Uri;
        /// <summary>
        ///     ��ǰ���õĻ������ö���
        /// </summary>
        public CacheSetting CacheSetting;
        /// <summary>
        ///     �����
        /// </summary>
        public string CacheKey;
        /// <summary>
        /// ִ��״̬
        /// </summary>
        public RouteStatus Status;
        /// <summary>
        ///     ����ֵ
        /// </summary>
        public string ResultMessage;
        
        /// <summary>
        ///     Http Header�е�Authorization��Ϣ
        /// </summary>
        public string Bearer;

        /// <summary>
        ///     ��ǰ������õ���������
        /// </summary>
        public string HostName;

        /// <summary>
        ///     ��ǰ������õ�API����
        /// </summary>
        public string ApiName;

        /// <summary>
        ///     HTTP method
        /// </summary>
        public string HttpMethod;
        /// <summary>
        ///     ·��������Ϣ
        /// </summary>
        public HostConfig RouteHost;


    }
    /// <inheritdoc />
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
        private HttpRouter(HttpContext context)
        {
            Data=new RouteData();
            HttpContext = context;
            Request = context.Request;
            Response = context.Response;
            HttpIoLog.OnBegin(Request);
        }

        /// <inheritdoc />
        /// <summary>������Դ</summary>
        protected override void OnDispose()
        {
            HttpIoLog.OnEnd(Data.Status, Data.ResultMessage);
        }

        /// <summary>
        ///     ִ��·�ɲ���
        /// </summary>
        public static void Todo(HttpContext context)
        {
            using (var router = new HttpRouter(context))
            {
                // ��������
                router.Call();
                // д�뷵��
                router.WriteResult();
            }
        }

        /// <summary>
        /// ����
        /// </summary>
        private void Call()
        {
            Data.Uri = Request.GetUri();
            Data.HttpMethod = Request.Method.ToUpper();

            // 1 ��ʼ��������Ϣ
            if (!InitializeContext())
                return;
            // 2 ������ٴ���
            if (RouteRuntime.CheckCache(Data.Uri, Data.Bearer, out Data.CacheSetting, out Data.CacheKey, ref Data.ResultMessage))
            {
                //�ҵ������ػ���
                Data.Status = RouteStatus.Cache;
                return;
            }

            //3 ��ȫ���
            if (!SecurityCheck()) return;
            //3 ��ʼ��·����Ϣ
            if (!InitializeRoute())
                return;
            //4 ����Զ�̻���
            Data.ResultMessage = Data.RouteHost.ByZero ? CallZero() : CallHttp();
            //5 Զ�̵���
            //����
            RouteRuntime.CacheData(Data);
        }

        /// <summary>
        ///     ��ʼ������������
        /// </summary>
        private bool InitializeContext()
        {
            string authorization = Request.Headers["Authorization"];
            if (string.IsNullOrWhiteSpace(authorization))
            {
                Data.Bearer = Request.Query["ClientKey"];
            }
            else
            {
                var aa = authorization.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                if (aa.Length != 2 || !string.Equals(aa[0], "Bearer", StringComparison.OrdinalIgnoreCase) ||
                    aa[1].Equals("null"))
                    Data.Bearer = null;
                else
                    Data.Bearer = aa[1];
            }
            ApiContext.Current.Request.Bear = Data.Bearer;
            ApiContext.Current.Request.RequestId = RandomOperate.Generate(8);
            ApiContext.Current.Request.Ip = HttpContext.Connection.RemoteIpAddress.ToString();
            ApiContext.Current.Request.Port = HttpContext.Connection.RemotePort.ToString();
            ApiContext.Current.Request.ServiceKey = ApiContext.MyServiceKey;
            ApiContext.Current.Request.ArgumentType = ArgumentType.Json;
            ApiContext.Current.Request.UserAgent = Request.Headers["User-Agent"];

            return true;
        }

        /// <summary>
        ///     ��ȫ���
        /// </summary>
        private bool SecurityCheck()
        {
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
            return false;
        }


        /// <summary>
        ///     д�뷵��
        /// </summary>
        private void WriteResult()
        {
            if (Data.Status >= RouteStatus.HttpRedirect)
                return;
            Response.Headers.Add("Access-Control-Allow-Origin", "*");
            Response.ContentType = "text/plain; charset=utf-8";
            if (Data.ResultMessage != null)
                Response.WriteAsync(Data.ResultMessage, Encoding.UTF8);
        }

        #endregion

        #region ·��

        /// <summary>
        ///     ��ʼ��·����Ϣ
        /// </summary>
        private bool InitializeRoute()
        {
            var words = Data.Uri.LocalPath.Split('/', 2, StringSplitOptions.RemoveEmptyEntries);
            Data.HostName = words[0];
            if (words.Length <= 1)
            {
                Data.Status = RouteStatus.FormalError;
                Data.ResultMessage = RouteRuntime.DenyAccess;
                return false;
            }

            Data.ApiName = words[1];
            if (!AppConfig.Config.RouteMap.TryGetValue(Data.HostName, out Data.RouteHost))
                Data.RouteHost = HostConfig.DefaultHost;
            return true;
            //StringBuilder sb = new StringBuilder();
            //int step = 0;
            //foreach (var ch in Data.Uri.PathAndQuery)
            //{
            //    switch (ch)
            //    {
            //        case '/':
            //            if (step == 0)
            //            {
            //                sb.Append(ch);
            //            }
            //            else if (sb.Length > 0)
            //            {
            //                isFirst = false;
            //                _model = sb.ToString();
            //                sb.Clear();
            //            }
            //            break;
            //        case '?':
            //            _api = sb.ToString();
            //            break;
            //        default:
            //            sb.Append(ch);
            //            break;
            //    }
            //}
            //if (_model == null)
            //{
            //    status != 0 = true;
            //    Data.ResultMessage = DenyAccess;
            //}
            //if (sb.Length > 0)
            //    _apiAndQuery = sb.ToString();
            //else
            //{
            //    status != 0 = true;
            //    Data.ResultMessage = DenyAccess;
            //}
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
            var req = caller.CreateRequest(httpApi, Data.HttpMethod, Request);

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