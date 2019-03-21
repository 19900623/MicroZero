using System;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace MicroZero.Http.Route
{
    /// <summary>
    ///     ��ά��������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract, Serializable]
    public class SmsConfig
    {

        /// <summary>
        ///     ÿ����������Сʱ��
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public int CycleHours;

        /// <summary>
        ///     ÿ������������෢�ʹ���
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public int CycleSendCount;

        /// <summary>
        ///     ���յĵ绰����
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string[] Phones;

        /// <summary>
        ///     �����accessKeyId
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliAccessKeyId;

        /// <summary>
        ///     �����accessKeySecret
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliAccessKeySecret;

        /// <summary>
        ///     ����API��Ʒ����
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliDomain;

        /// <summary>
        ///     �ڵ�
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliEndPointName;

        /// <summary>
        ///     ����API��Ʒ����
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliProduct;

        /// <summary>
        ///     ����
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliRegionId;

        /// <summary>
        ///     ǩ��
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliSignName;

        /// <summary>
        ///     ģ������
        /// </summary>
        [DataMember, JsonProperty(NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public string AliTemplateCode;
    }
}