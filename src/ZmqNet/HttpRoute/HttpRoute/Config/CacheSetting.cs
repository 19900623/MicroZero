using System;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ��������
    /// </summary>
    [Flags]
    internal enum CacheFeature : uint
    {
        /// <summary>
        /// ��
        /// </summary>
        None,

        /// <summary>
        /// ���ͷ��ͬ
        /// </summary>
        Bear = 0x1,

        /// <summary>
        /// �����������ʱ
        /// </summary>
        NetError = 0x2,

        /// <summary>
        /// ������ͬ
        /// </summary>
        QueryString = 0x4,

    }
    /// <summary>
    /// ��������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract]
    internal class CacheSetting
    {
        /// <summary>
        /// API����
        /// </summary>
        [DataMember, JsonProperty]
        public string Api { get; set; }

        /// <summary>
        /// ����У������ͷ
        /// </summary>
        [DataMember, JsonProperty]
        public string Bear { get; set; }

        /// <summary>
        /// ���������
        /// </summary>
        [DataMember, JsonProperty]
        public int FlushSecond { get; set; }

        /// <summary>
        /// ����ʱ��ʹ�����ƣ����������ѯ�ַ�����
        /// </summary>
        [DataMember, JsonProperty]
        public bool OnlyName { get; set; }

        /// <summary>
        /// �����������ʱ����
        /// </summary>
        [DataMember, JsonProperty]
        public bool ByNetError { get; set; }

        /// <summary>
        /// ��������
        /// </summary>
        [IgnoreDataMember , JsonIgnore]
        public CacheFeature Feature { get; private set; }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        public void Initialize()
        {
            //Ĭ��5����
            if (FlushSecond <= 0)
                FlushSecond = 300;
            else if (FlushSecond > 3600)
                FlushSecond = 3600;
            if (!string.IsNullOrWhiteSpace(Bear))
                Feature |= CacheFeature.Bear;
            if(!OnlyName)
                Feature |= CacheFeature.QueryString;
            if(ByNetError)
                Feature |= CacheFeature.NetError;
        }
    }
    /// <summary>
    /// ��������
    /// </summary>
    internal class CacheData
    {
        /// <summary>
        /// �´θ���ʱ��
        /// </summary>
        public DateTime UpdateTime { get; set; }
        /// <summary>
        /// ��������
        /// </summary>
        public string Content { get; set; }
    }
}