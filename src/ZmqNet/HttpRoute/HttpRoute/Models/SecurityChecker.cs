using System;
using System.Configuration;
using System.Linq;
using Agebull.Common.Logging;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Microsoft.AspNetCore.Http;
using Yizuan.Service.Api;
using Agebull.Common.DataModel.Redis;
using GoodLin.Common.Redis;

namespace Yizuan.Service.Host
{
    /// <summary>
    /// ��ȫ���Ա
    /// </summary>
    public class SecurityChecker
    {
        private static bool CheckBearCache = bool.Parse(ConfigurationManager.AppSettings["CheckBearCache"] ?? "true");
        /// <summary>
        /// Http����
        /// </summary>
        public HttpRequest Request { get; set; }

        /// <summary>
        /// Authͷ
        /// </summary>
        public string Bear { get; set; }

        /// <summary>
        /// �������
        /// </summary>
        public int Status { get; set; }


        /// <summary>
        ///     ִ�м��
        /// </summary>
        /// <returns>
        ///     0:��ʾͨ����֤�����Լ���
        ///     1������Ϊ�ջ򲻺ϸ�
        ///     2��������α���
        /// </returns>
        public bool Check()
        {
            if (string.IsNullOrWhiteSpace(Bear))
            {
                return Request.GetUri().LocalPath.Contains("/oauth/getdid");
            }
            //var header = Request.Headers.Values.LinkToString(" ");
            //if (string.IsNullOrWhiteSpace(header) || header.Contains("iToolsVM"))
            //    return false;

            switch (Bear[0])
            {
                default:
                    return false;
                case '*':
                    return CheckDeviceId();
                case '{':
                    return false;
                case '$':
                    return false;
                case '#':
                    return CheckAccessToken();
            }
        }

        /// <summary>
        ///     ����豸��ʶ
        /// </summary>
        /// <returns>
        ///     0:��ʾͨ����֤�����Լ���
        ///     1������Ϊ��
        ///     2��������α���
        /// </returns>
        private bool CheckDeviceId()
        {
            for (var index = 1; index < Bear.Length; index++)
            {
                var ch = Bear[index];
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_')
                    continue;
                Status = ErrorCode.Auth_Device_Unknow;
                return false;
            }
            if (!CheckBearCache)
                return true;
            using (var proxy = new RedisProxy(RedisProxy.DbAuthority))
            {
               return proxy.Client.KeyExists(RedisKeyBuilder.ToAuthKey("token", "did", Bear));
            }
        }

        /// <summary>
        ///     ���AccessToken
        /// </summary>
        /// <returns>
        ///     0:��ʾͨ����֤�����Լ���
        ///     1������Ϊ��
        ///     2��������α���
        /// </returns>
        private bool CheckAccessToken()
        {
            if (Bear.Length != 33)
                return false;
            for (var index = 1; index < Bear.Length; index++)
            {
                var ch = Bear[index];
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
                    continue;
                Status = ErrorCode.Auth_AccessToken_Unknow;
                return false;
            }
            if (!CheckBearCache)
                return true;
            using (var proxy = new RedisProxy(RedisProxy.DbAuthority))
            {
                return proxy.Client.KeyExists(RedisKeyBuilder.ToAuthKey("at", Bear));
            }
        }
    }
}