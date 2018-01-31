using System;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// ����վ������
    /// </summary>
    [Serializable]
    [JsonObject(MemberSerialization.OptIn)]
    public class LocalStationConfig
    {
        /// <summary>
        /// ��������
        /// </summary>
        [DataMember, JsonProperty("serviceName")]
        public string ServiceName { get; set; }

        /// <summary>
        /// վ�����ƣ�ע��Ψһ��
        /// </summary>
        [DataMember, JsonProperty("stationName")]
        public string StationName { get; set; }

        /// <summary>
        /// ZeroNet��Ϣ��������IP��ַ
        /// </summary>
        [DataMember, JsonProperty("zeroAddr")]
        public string ZeroAddress { get; set; }

        /// <summary>
        /// ZeroNet��Ϣ���ļ��վ�˿ں�
        /// </summary>
        [DataMember, JsonProperty("monitorPort")]
        public int ZeroMonitorPort { get; set; }

        /// <summary>
        /// ZeroNet��Ϣ���Ĺ���վ�˿ں�
        /// </summary>
        [DataMember, JsonProperty("managePort")]
        public int ZeroManagePort { get; set; }

    }
}