using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// �����ڵ�
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract]
    public class WaringItem
    {
        /// <summary>
        /// API����
        /// </summary>
        [DataMember, JsonProperty("host")] public string Host;
        [DataMember, JsonProperty("api")]
        public string Api;

        /// <summary>
        /// ��Ϣ
        /// </summary>
        [DataMember, JsonProperty("message")]
        public string Message;
    }
}