using System;
using System.Net;
using System.Text;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Microsoft.AspNetCore.Http;
using Newtonsoft.Json;
using Yizuan.Service.Host;
using Agebull.Common;
using Yizuan.Service.Api;
using Agebull.Common.Logging;
using Microsoft.AspNetCore.Server.Kestrel.Transport.Libuv.Internal;

namespace ExternalStation.Models
{
    /// <summary>
    /// ����ӳ�������
    /// </summary>
    public class HttpRouter
    {
        #region ����

        /// <summary>
        /// Http������
        /// </summary>
        public HttpContext HttpContext { get; }
        /// <summary>
        /// Http����
        /// </summary>
        public HttpRequest Request { get; }
        /// <summary>
        /// Http����
        /// </summary>
        public HttpResponse Response { get; }


        /// <summary>
        /// Http Header�е�Authorization��Ϣ
        /// </summary>
        string _bear;

        /// <summary>
        /// ��ǰ·��
        /// </summary>
        private Uri _callUri;

        /// <summary>
        /// ��ǰ������õ�ģ�Ͷ�Ӧ����������
        /// </summary>
        private string _host;

        /// <summary>
        /// ��ǰ������õ�ģ������
        /// </summary>
        private string _model;

        /// <summary>
        /// HTTP method
        /// </summary>
        private string _httpMethod;

        /// <summary>
        /// ��ǰ������õ�Api����
        /// </summary>
        private string _apiAndQuery;

        /// <summary>
        /// ��ʧ��
        /// </summary>
        private bool _isFailed;

        /// <summary>
        /// ����ֵ
        /// </summary>
        private string _resultMessage;


        #endregion

        #region Ĭ�Ϸ�������

        /// <summary>
        /// �ܾ����ʵ�Json�ַ���
        /// </summary>
        private static readonly string DenyAccess = JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.DenyAccess));

        /// <summary>
        /// �ܾ����ʵ�Json�ַ���
        /// </summary>
        private static readonly string DeviceUnknow = JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.Auth_Device_Unknow));

        /// <summary>
        /// �ܾ����ʵ�Json�ַ���
        /// </summary>
        private static readonly string TokenUnknow = JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.Auth_AccessToken_Unknow));

        /// <summary>
        /// �����ɹ����ַ���
        /// </summary>
        private static readonly string DefaultSuccees = JsonConvert.SerializeObject(ApiResult.Succees());


        #endregion


        /// <summary>
        /// �ڲ�����
        /// </summary>
        /// <param name="context"></param>
        private HttpRouter(HttpContext context)
        {
            HttpContext = context;
            Request = context.Request;
            Response = context.Response;
        }

        /// <summary>
        /// ִ��·�ɲ���
        /// </summary>
        public static void Todo(HttpContext context)
        {
            HttpRouter router = new HttpRouter(context);
            router.DoRoute();
        }
        /// <summary>
        /// ִ��·�ɲ���
        /// </summary>
        private void DoRoute()
        {
            _httpMethod = Request.Method.ToUpper();
            //����֧��
            if (_httpMethod == "OPTIONS")
            {
                Cros();
                return;
            }
            //����Ϊ��ȷ����
            HttpIoLog.OnBegin(Request);
            Call();
            WriteResult();
            HttpIoLog.OnEnd(_resultMessage ?? DenyAccess);
        }

        /// <summary>
        /// д�뷵��
        /// </summary>
        void WriteResult()
        {
            Response.Headers.Add("Access-Control-Allow-Origin", "*");
            Response.ContentType = "text/plain";
            Response.WriteAsync(_resultMessage ?? DenyAccess, Encoding.UTF8);
            //task.Wait();
        }

        void Call()
        {
            // 1 ��ʼ��������Ϣ
            InitializeBaseContext();
            if (_isFailed)
                return;
            // 2 ������ٴ���
            if (CheckCache())
                return;
            //3 ��ȫ���
            SecurityCheck();
            if (_isFailed)
                return;
            //3 ��ʼ��·����Ϣ
            InitializeRoute();
            if (_isFailed)
                return;
            //4 ����Զ�̻���
            FindHost();
            if (_isFailed)
                return;
            //5 Զ�̵���
            _resultMessage = CallRemote();
            //����
            CacheData();
        }

        /// <summary>
        /// ��ʼ������������
        /// </summary>
        void InitializeBaseContext()
        {
            _callUri = Request.GetUri();

            string authorization = Request.Headers["Authorization"];
            if (string.IsNullOrWhiteSpace(authorization) ||
                string.Equals(authorization, "Bear null", StringComparison.OrdinalIgnoreCase))
            {
                _bear = null;
            }
            else
            {
                var aa = authorization.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                if (aa.Length != 2 || !string.Equals(aa[0], "Bear", StringComparison.OrdinalIgnoreCase))
                    _bear = null;
                else
                    _bear = aa[1];
            }
        }
        /// <summary>
        /// ��ȫ���
        /// </summary>
        void SecurityCheck()
        {
            SecurityChecker checker = new SecurityChecker
            {
                Request = Request,
                Bear = _bear
            };
            if (checker.Check())
                return;
            _isFailed = true;
            switch (checker.Status)
            {
                case ErrorCode.Auth_Device_Unknow:
                    _resultMessage = DeviceUnknow;
                    break;
                case ErrorCode.Auth_AccessToken_Unknow:
                    _resultMessage = TokenUnknow;
                    break;
            }
        }

        /// <summary>
        /// ��ʼ��·����Ϣ
        /// </summary>
        private void InitializeRoute()
        {
            StringBuilder sb = new StringBuilder();
            bool isFirst = true;
            foreach (var ch in _callUri.PathAndQuery)
            {
                switch (ch)
                {
                    case '/':
                        if (!isFirst)
                        {
                            sb.Append(ch);
                        }
                        else if (sb.Length > 0)
                        {
                            isFirst = false;
                            _model = sb.ToString();
                            sb.Clear();
                        }
                        break;
                    default:
                        sb.Append(ch);
                        break;
                }
            }
            if (_model == null)
            {
                _isFailed = true;
                _resultMessage = DenyAccess;
            }
            if (sb.Length > 0)
                _apiAndQuery = sb.ToString();
            else
            {
                _isFailed = true;
                _resultMessage = DenyAccess;
            }
        }

        /// <summary>
        /// ��������
        /// </summary>
        private void FindHost()
        {
            RouteHost hostData;
            if (!RouteData.HostMap.TryGetValue(_model, out hostData) || hostData.Hosts.Length == 0)
            {
                lock (RouteData.DefaultHostData)//ƽ������
                {
                    _host = RouteData.DefaultHostData.Hosts[RouteData.DefaultHostData.Next] + _model;
                    if (++RouteData.DefaultHostData.Next >= RouteData.DefaultHostData.Hosts.Length)
                    {
                        RouteData.DefaultHostData.Next = 0;
                    }
                }
            }
            else if (hostData.Hosts.Length == 1)
            {
                _host = hostData.Hosts[0];
            }
            else
            {
                lock (hostData)//ƽ������
                {
                    _host = hostData.Hosts[hostData.Next];
                    if (++hostData.Next >= hostData.Hosts.Length)
                    {
                        hostData.Next = 0;
                    }
                }
            }
        }


        #region ����

        /// <summary>
        /// ��ǰ���õĻ������ö���
        /// </summary>
        CacheSetting _cacheSetting;

        /// <summary>
        /// �����
        /// </summary>
        string _cacheKey;

        CacheData cacheData;

        /// <summary>
        /// ��黺��
        /// </summary>
        /// <returns>ȡ�����棬����ֱ�ӷ���</returns>
        private bool CheckCache()
        {
            if (!RouteData.CacheMap.TryGetValue(_callUri.LocalPath, out _cacheSetting))
                return false;

            if (_cacheSetting.Feature.HasFlag(CacheFeature.Bear) &&
                _bear.Substring(0, _cacheSetting.Bear.Length) != _cacheSetting.Bear)
            {
                _cacheSetting = null;
                return false;
            }

            _cacheKey = _cacheSetting.OnlyName ? _callUri.LocalPath : _callUri.PathAndQuery;
            lock (_cacheSetting)
            {
                if (!RouteData.Cache.TryGetValue(_cacheKey, out cacheData))
                    return false;
            }
            if (cacheData.UpdateTime <= DateTime.Now)
                return false;
            _resultMessage = cacheData.Content;
            return true;
        }
        /// <summary>
        /// ��������
        /// </summary>
        private void CacheData()
        {
            if (_cacheSetting == null)
                return;
            if (cacheData == null)
                cacheData = new CacheData
                {
                    Content = _resultMessage
                };
            if (_cacheSetting.Feature.HasFlag(CacheFeature.NetError) && WebStatus != WebExceptionStatus.Success)
            {
                cacheData.UpdateTime = DateTime.Now.AddSeconds(30);
            }
            else
            {
                cacheData.UpdateTime = DateTime.Now.AddSeconds(_cacheSetting.FlushSecond);
            }

            lock (RouteData.Cache)
            {
                if (!RouteData.Cache.ContainsKey(_cacheKey))
                    RouteData.Cache.Add(_cacheKey, cacheData);
                else
                    RouteData.Cache[_cacheKey] = cacheData;
            }
        }

        #endregion
        #region Զ�̵���
        /// <summary>
        /// Զ�̵���״̬
        /// </summary>
        WebExceptionStatus WebStatus;
        /// <summary>
        /// Զ�̵���
        /// </summary>
        /// <returns></returns>
        private string CallRemote()
        {
            var callContext = new InternalCallContext
            {
                ServiceKey = RouteData.ServiceKey,
                RequestId = Guid.NewGuid(),
                Bear = _bear
            };
            ApiContext.SetRequestContext(callContext);
            ApiContext.Current.Cache();
            var caller = new ApiCaller(_host)
            {
                Bearer = "Bear " + ApiContext.RequestContext.Bear
            };
            var req = caller.CreateRequest(_apiAndQuery, _httpMethod, _httpMethod == "POST" && Request.ContentLength > 0 ? Request.Form : null);

            return caller.GetResult(req, out WebStatus);
        }
        #endregion
        #region ����֧��

        /// <summary>
        /// ����֧��
        /// </summary>
        void Cros()
        {
            Response.Headers.Add("Access-Control-Allow-Methods", new[] { "GET", "POST" });
            Response.Headers.Add("Access-Control-Allow-Headers", new[] { "x-requested-with", "content-type", "authorization", "*" });
            Response.Headers.Add("Access-Control-Allow-Origin", "*");
        }

        #endregion
    }
}