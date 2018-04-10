using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using Agebull.Common;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·������
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract]
    public class AppConfig
    {
        public static AppConfig Config { get; set; }

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
        [DataMember, JsonProperty("smsConfig")] private SmsConfig _smsConfig;

        /// <summary>
        /// ��ά��������
        /// </summary>
        public SmsConfig SmsConfig => _smsConfig ?? (_smsConfig = new SmsConfig());

        /// <summary>
        /// ϵͳ����
        /// </summary>
        [DataMember, JsonProperty("sysConfig")] private SystemConfig _systemConfig;

        /// <summary>
        /// ϵͳ����
        /// </summary>
        public SystemConfig SystemConfig => _systemConfig ?? (_systemConfig = new SystemConfig());

        /// <summary>
        /// ��������
        /// </summary>
        [DataMember, JsonProperty("cache")] private List<CacheSetting> _cacheSettings;

        /// <summary>
        /// ·������
        /// </summary>
        public Dictionary<string, CacheSetting> CacheMap { get; set; }

        /// <summary>
        /// ·������
        /// </summary>
        [DataMember, JsonProperty("route")] private Dictionary<string, HostConfig> _routeConfig;

        /// <summary>
        /// ����ͼ
        /// </summary>
        public Dictionary<string, HostConfig> RouteMap { get; private set; }

        /// <summary>
        /// �ļ�����
        /// </summary>
        public static string FileName { get; private set; }
        /// <summary>
        /// ��ȫ����
        /// </summary>
        [DataMember, JsonProperty("security")]
        public SecurityConfig Security { get; set; }
        /// <summary>
        /// �Ƿ��ѳ�ʼ��
        /// </summary>
        public static bool  IsInitialized { get; private set; }
        /// <summary>
        /// ��ʼ��
        /// </summary>
        /// <returns></returns>
        public static bool Initialize(string file)
        {
            FileName = file;
            return Initialize();
        }

        /// <summary>
        /// ��ʼ��
        /// </summary>
        /// <returns></returns>
        public static bool Initialize()
        {
            if (!File.Exists(FileName))
                return false;
            Config = JsonConvert.DeserializeObject<AppConfig>(File.ReadAllText(FileName));
            if (Config == null)
                return false;
            Config.StationConfig.DataFolder = IOHelper.CheckPath(Path.GetDirectoryName(FileName),"Datas");
            LogRecorder.LogMonitor = Config.SystemConfig.LogMonitor;
            Config.InitCache();
            Config.InitRoute();
            Config.InitCheckApis();
            IsInitialized = true;
            return HostConfig.DefaultHost != null;
        }
        /// <summary>
        /// ��ʼ��·��
        /// </summary>
        /// <returns></returns>
        public void InitCheckApis()
        {
            Config.Security.CheckApis = new Dictionary<string, ApiItem>(StringComparer.OrdinalIgnoreCase);
            foreach (var apiItem in Config.Security.checkApis)
            {
                Config.Security.CheckApis.Add(apiItem.Key, apiItem.Value);
            }
            Config.Security.DenyTokens = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (var apiItem in Config.Security.denyTokens)
            {
                Config.Security.DenyTokens.Add(apiItem.Key, apiItem.Value);
            }
        }

        /// <summary>
        /// ��ʼ��·��
        /// </summary>
        /// <returns></returns>
        public Dictionary<string, CacheSetting> InitCache()
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
        public Dictionary<string, HostConfig> InitRoute()
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