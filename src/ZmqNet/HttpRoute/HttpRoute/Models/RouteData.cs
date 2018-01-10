using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace ExternalStation.Models
{
    /// <summary>
    /// ·������
    /// </summary>
    [Serializable]
    [JsonObject(MemberSerialization.OptIn)]
    internal class RouteHost
    {
        /// <summary>
        /// ʹ��ZeroNetͨѶ��
        /// </summary>
        [DataMember, JsonProperty("zero", NullValueHandling = NullValueHandling.Ignore, DefaultValueHandling = DefaultValueHandling.Ignore)]
        public bool ByZero { get; set; }

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

    /// <summary>
    /// ·����ص�����
    /// </summary>
    public class RouteData
    {

        /// <summary>
        /// ��ǰ������Key
        /// </summary>
        internal static string ServiceKey;
        /// <summary>
        /// ��������ͼ
        /// </summary>
        internal static Dictionary<string, RouteHost> HostMap;

        /// <summary>
        /// ����ͼ
        /// </summary>
        internal static Dictionary<string, CacheSetting> CacheMap;

        /// <summary>
        /// Ĭ������
        /// </summary>
        internal static RouteHost DefaultHostData;
        /// <summary>
        /// ��������
        /// </summary>
        internal static Dictionary<string, CacheData> Cache;


        /// <summary>
        /// ˢ��
        /// </summary>
        public static void Flush()
        {
            ServiceKey = ConfigurationManager.AppSettings["ServiceKey"] ?? "HttpRouter";

            var rootPath = Startup.Configuration["contentRoot"];
            var file = Path.Combine(rootPath, "machine.json");
            var json = File.ReadAllText(file);
            var map1 = JsonConvert.DeserializeObject<Dictionary<string, RouteHost>>(json);
            HostMap = new Dictionary<string, RouteHost>(StringComparer.OrdinalIgnoreCase);
            if (map1 != null)
            {
                foreach (var kv in map1)
                {
                    if (kv.Key == "Default")
                    {
                        DefaultHostData = kv.Value;
                    }
                    else
                    {
                        if (!kv.Value.ByZero && (kv.Value.Hosts == null || kv.Value.Hosts.Length == 0))
                            continue;
                        HostMap.Add(kv.Key, kv.Value);
                        if (kv.Value.Alias != null)
                        {
                            foreach(var name in kv.Value.Alias)
                                HostMap.Add(name, kv.Value);
                        }
                    }
                }
            }
            file = Path.Combine(rootPath, "cache.json");
            json = File.ReadAllText(file);
            var cs = JsonConvert.DeserializeObject<List<CacheSetting>>(json);
            CacheMap = new Dictionary<string, CacheSetting>();
            if (cs != null)
            {
                foreach (var c in cs)
                {
                    c.Initialize();
                    CacheMap.Add(c.Api, c);
                }
            }
            Cache = new Dictionary<string, CacheData>(StringComparer.OrdinalIgnoreCase);
        }
    }
}