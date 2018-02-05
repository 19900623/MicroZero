using System;

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
        /// ִ��HTTP��д����
        /// </summary>
        public bool Redirect;
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
        ///     �������������
        /// </summary>
        public string Context;

        /// <summary>
        ///     HTTP method
        /// </summary>
        public string HttpMethod;
        /// <summary>
        ///     ·��������Ϣ
        /// </summary>
        public HostConfig RouteHost;

        /// <summary>
        /// �Ƿ�����
        /// </summary>
        public bool IsSucceed { get; set; }
    }
}