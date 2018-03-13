using System;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract, Serializable]
    internal class HostConfig
    {
        /// <summary>
        /// Ĭ������
        /// </summary>
        internal static HostConfig DefaultHost;
#if ZERO
        /// <summary>
        /// ʹ��ZeroNetͨѶ��
        /// </summary>
        [DataMember, JsonProperty("zero", NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public bool ByZero { get; set; }
#endif

        /// <summary>
        /// ��һ�����е�����
        /// </summary>
        [IgnoreDataMember, JsonIgnore]
        public int Next { get; set; }

        /// <summary>
        /// �����б�
        /// </summary>
        [DataMember, JsonProperty("hosts", NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string[] Hosts { get; set; }

        /// <summary>
        /// ����
        /// </summary>
        [DataMember, JsonProperty("alias", NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string[] Alias { get; set; }

    }
}