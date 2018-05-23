using System;
using System.Runtime.Serialization;
using System.Text;
using Gboxt.Common.DataModel;
using Newtonsoft.Json;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// 本地站点配置
    /// </summary>
    [Serializable]
    [JsonObject(MemberSerialization.OptIn)]
    public class LocalStationConfig
    {
        /// <summary>
        /// 服务名称
        /// </summary>
        [DataMember, JsonProperty("serviceName")]
        public string ServiceName { get; set; }

        /// <summary>
        /// 站点名称，注意唯一性
        /// </summary>
        [DataMember, JsonProperty("stationName")]
        public string StationName { get; set; }

        /// <summary>
        /// ZeroNet消息中心主机IP地址
        /// </summary>
        [DataMember, JsonProperty("zeroAddr")]
        public string ZeroAddress { get; set; }

        /// <summary>
        /// ZeroNet消息中心监测站端口号
        /// </summary>
        [DataMember, JsonProperty("monitorPort")]
        public int ZeroMonitorPort { get; set; }

        /// <summary>
        /// ZeroNet消息中心管理站端口号
        /// </summary>
        [DataMember, JsonProperty("managePort")]
        public int ZeroManagePort { get; set; }

        /// <summary>
        /// 本地数据文件夹
        /// </summary>
        [DataMember, JsonProperty("dataFolder")]
        public string DataFolder { get; set; }

        /// <summary>
        /// 实例名称
        /// </summary>
        [DataMember, JsonProperty("realName")]
        private string _realName;
        /// <summary>
        /// 实例名称
        /// </summary>
        [DataMember, JsonProperty("serviceKey")]
        private string _serviceKey;

        /// <summary>
        /// 实例名称
        /// </summary>
        [IgnoreDataMember, JsonIgnore]
        public string ServiceKey
        {
            get
            {
                if (!string.IsNullOrEmpty(_serviceKey))
                    return _serviceKey;
                return _serviceKey = RandomOperate.Generate(8);
            }
        }

        /// <summary>
        /// 实例名称
        /// </summary>
        [IgnoreDataMember, JsonIgnore]
        public string RealName
        {
            get
            {
                if (!string.IsNullOrEmpty(_realName))
                    return _realName;
                return _realName = ZeroIdentityHelper.CreateRealName();
            }
        }

        private byte[] _identity;

        /// <summary>
        /// 实例名称
        /// </summary>
        [IgnoreDataMember, JsonIgnore]
        public byte[] Identity => _identity ?? (_identity = ZeroIdentityHelper.ToZeroIdentity());

    }
}