using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using Microsoft.ApplicationInsights.AspNetCore.Extensions;
using Microsoft.AspNetCore.Http;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract]
    public class RouteData
    {
        /// <summary>
        /// ��ʼʱ��
        /// </summary>
        [DataMember, JsonProperty("start")]
        public DateTime Start { get; set; }

        /// <summary>
        /// ����ʱ��
        /// </summary>
        [DataMember, JsonProperty("end")]
        public DateTime End { get; set; }

        /// <summary>
        /// �����ַ
        /// </summary>
        [DataMember, JsonProperty("uri")]
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
        [DataMember, JsonProperty("status")]
        public RouteStatus Status;
        /// <summary>
        /// ִ��HTTP��д����
        /// </summary>
        [DataMember, JsonProperty("redirect")]
        public bool Redirect;
        /// <summary>
        ///     ����ֵ
        /// </summary>
        [DataMember, JsonProperty("result")]
        public string ResultMessage;

        /// <summary>
        ///     Http Header�е�Authorization��Ϣ
        /// </summary>
        [DataMember, JsonProperty("bear")]
        public string Bearer;

        /// <summary>
        ///     ��ǰ������õ���������
        /// </summary>
        [DataMember, JsonProperty("host")]
        public string HostName;

        /// <summary>
        ///     ��ǰ������õ�API����
        /// </summary>
        [DataMember, JsonProperty("apiName")]
        public string ApiName;

        /// <summary>
        ///     ���������
        /// </summary>
        [DataMember, JsonProperty("context")]
        public string Context;

        /// <summary>
        ///     ����Ĳ���
        /// </summary>
        [DataMember, JsonProperty("queryString")]
        public string QueryString;

        /// <summary>
        ///     ����ı�
        /// </summary>
        [DataMember, JsonProperty("headers")]
        public Dictionary< string,List<string>> Headers = new Dictionary<string, List<string>>(StringComparer.OrdinalIgnoreCase);

        /// <summary>
        ///     ����ı�
        /// </summary>
        [DataMember, JsonProperty("form")]
        public string Form;
        
        /// <summary>
        ///     HTTP method
        /// </summary>
        [DataMember, JsonProperty("method")]
        public string HttpMethod;
        /// <summary>
        ///     ·��������Ϣ
        /// </summary>
        public HostConfig RouteHost;

        /// <summary>
        /// �Ƿ�����
        /// </summary>
        [DataMember, JsonProperty("succeed")]
        public bool IsSucceed { get; set; }

        /// <summary>
        /// ׼��
        /// </summary>
        /// <param name="request"></param>
        public void Prepare(HttpRequest request)
        {
            Uri = request.GetUri();
            foreach (var head in request.Headers)
            {
                Headers.Add(head.Key, head.Value.ToList());
            }
            HttpMethod = request.Method.ToUpper();
            if (request.QueryString.HasValue)
            {
                Form = request.QueryString.ToString();
            }
            if (request.HasFormContentType)
            {
                Form = request.Form.LinkToString(p => $"{p.Key}={p.Value}", "&");
            }
            else if (request.ContentLength != null)
            {
                using (var texter = new StreamReader(request.Body))
                {
                    Context = texter.ReadToEnd();
                }
            }
        }
    }
}