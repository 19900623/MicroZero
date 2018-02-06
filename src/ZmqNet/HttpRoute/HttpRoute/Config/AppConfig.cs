using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn),DataContract]
    internal class AppConfig
    {
        internal static AppConfig Config { get; set; }

        /// <summary>
        /// վ������
        /// </summary>
        [DataMember, JsonProperty("zeroConfig")]
        public LocalStationConfig StationConfig
        {
            get => StationProgram.Config;
            set => StationProgram.Config = value;
        }

        /// <summary>
        /// ��ά��������
        /// </summary>
        [DataMember, JsonProperty("smsConfig")] private SmsConfig smsConfig;

        /// <summary>
        /// ��ά��������
        /// </summary>
        public SmsConfig SmsConfig => smsConfig ?? (smsConfig = new SmsConfig());

        /// <summary>
        /// ϵͳ����
        /// </summary>
        [DataMember, JsonProperty("sysConfig")] private SystemConfig systemConfig;

        /// <summary>
        /// ϵͳ����
        /// </summary>
        internal SystemConfig SystemConfig => systemConfig ?? (systemConfig = new SystemConfig());

        /// <summary>
        /// ��������
        /// </summary>
        [DataMember, JsonProperty("cache")] private List<CacheSetting> _cacheSettings;
        
        /// <summary>
        /// ·������
        /// </summary>
        internal Dictionary<string, CacheSetting> CacheMap { get; set; }

        /// <summary>
        /// ·������
        /// </summary>
        [DataMember, JsonProperty("route")] private Dictionary<string, HostConfig> _routeConfig;

        /// <summary>
        /// ����ͼ
        /// </summary>
        internal Dictionary<string, HostConfig> RouteMap { get; private set; }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        /// <returns></returns>
        public static bool Initialize()
        {
            var file = Path.Combine(Startup.Configuration["contentRoot"], "route_config.json");
            if (!File.Exists(file))
                return false;
            Config = JsonConvert.DeserializeObject<AppConfig>(File.ReadAllText(file));
            if (Config == null)
                return false;
            LogRecorder.LogMonitor = Config.SystemConfig.LogMonitor;
            Config.InitCache();
            Config.InitRoute();
            Config.InitCheckApis();

            return HostConfig.DefaultHost != null;
        }
        /// <summary>
        /// ��ʼ��·��
        /// </summary>
        /// <returns></returns>
        internal void InitCheckApis()
        {
            var map = Config.SystemConfig.CheckApis;
            Config.SystemConfig.CheckApis = new Dictionary<string, ApiItem>(StringComparer.OrdinalIgnoreCase);
            if (map == null || map.Count == 0)
                return;
            foreach (var setting in map)
            {
                Config.SystemConfig.CheckApis.Add(setting.Key, setting.Value);
            }
        }

        /// <summary>
        /// ��ʼ��·��
        /// </summary>
        /// <returns></returns>
        internal Dictionary<string, CacheSetting> InitCache()
        {
            CacheMap = new Dictionary<string, CacheSetting>(StringComparer.OrdinalIgnoreCase);
            if (_cacheSettings == null)
                return CacheMap;
            foreach (var setting in _cacheSettings)
            {
                setting.Initialize();
                if (!CacheMap.ContainsKey(setting.Api))
                    CacheMap.Add(setting.Api, setting);
                else
                    CacheMap[setting.Api] = setting;
            }
            return CacheMap;
        }

        /// <summary>
        /// ��ʼ��·��
        /// </summary>
        /// <returns></returns>
        internal Dictionary<string, HostConfig> InitRoute()
        {
            RouteMap = new Dictionary<string, HostConfig>(StringComparer.OrdinalIgnoreCase);
            if (_routeConfig == null)
                return RouteMap;

            foreach (var kv in _routeConfig)
            {
                if (String.Equals(kv.Key, "Default", StringComparison.OrdinalIgnoreCase))
                {
                    HostConfig.DefaultHost = kv.Value;
                    continue;
                }
                if (!kv.Value.ByZero && (kv.Value.Hosts == null || kv.Value.Hosts.Length == 0))
                    continue;
                //Http����
                if (!RouteMap.ContainsKey(kv.Key))
                    RouteMap.Add(kv.Key, kv.Value);
                else
                    RouteMap[kv.Key] = kv.Value;
                //����
                if (kv.Value.Alias == null)
                    continue;
                foreach (var name in kv.Value.Alias)
                {
                    if (!RouteMap.ContainsKey(name))
                        RouteMap.Add(name, kv.Value);
                    else
                        RouteMap[name] = kv.Value;
                }
            }

            return RouteMap;
        }
    }
}