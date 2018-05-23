using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using Agebull.Common.Logging;
using Agebull.ZeroNet.PubSub;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·�ɼ�����
    /// </summary>
    internal class PerformanceCounter : SubStation
    {
        /// <summary>
        /// ����
        /// </summary>
        public PerformanceCounter()
        {
            StationName = "HealthCenter";
            Subscribe = "PerformanceCounter";
        }
        /// <summary>
        /// ִ������
        /// </summary>
        /// <param name="args"></param>
        /// <returns></returns>
        public override void Handle(PublishItem args)
        {
            try
            {
                if (args.SubTitle == "Save")
                {
                    Save();
                    return;
                }
                try
                {
                    var data = JsonConvert.DeserializeObject<RouteData>(args.Content);
                    End(data);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
            }
        }
        /// <summary>
        /// ��¼
        /// </summary>
        /// <param name="arg"></param>
        public static void Record(string arg)
        {
        }
        /// <summary>
        /// ������Ԫ
        /// </summary>
        public static long Unit = DateTime.Today.Year * 1000000 + DateTime.Today.Month * 10000 + DateTime.Today.Day * 100 + DateTime.Now.Hour;

        /// <summary>
        /// ������
        /// </summary>
        public static CountItem Station { get; set; } = new CountItem();
        
        /// <summary>
        /// ��ʼ����
        /// </summary>
        /// <returns></returns>
        public static void End(RouteData data)
        {
            try
            {
                var tm = (data.End - data.Start).TotalMilliseconds;
                if (tm > 200)
                    LogRecorder.Warning($"{data.HostName}/{data.ApiName}:ִ��ʱ���쳣({tm:F2}ms):");

                if (tm > AppConfig.Config.SystemConfig.WaringTime)
                    RuntimeWaring.Waring(data.HostName, data.ApiName, $"ִ��ʱ���쳣({tm:F0}ms)");

                long unit = DateTime.Today.Year * 1000000 + DateTime.Today.Month * 10000 + DateTime.Today.Day * 100 + DateTime.Now.Hour ;
                if (unit != Unit)
                {
                    Unit = unit;
                    Save();
                    Station = new CountItem();
                }

                Station.SetValue(tm, data);
                if (string.IsNullOrWhiteSpace(data.HostName))
                    return;
                CountItem host;
                lock (Station)
                {
                    if (!Station.Items.TryGetValue(data.HostName, out host))
                        Station.Items.Add(data.HostName, host = new CountItem());
                }
                host.SetValue(tm,data);

                if (string.IsNullOrWhiteSpace(data.ApiName))
                    return;
                CountItem api;
                lock (host)
                {
                    if (!host.Items.TryGetValue(data.ApiName, out api))
                        host.Items.Add(data.ApiName, api = new CountItem());
                }
                api.SetValue(tm, data);
            }
            catch (Exception e)
            {
                LogRecorder.Exception( e);
            }
        }


        /// <summary>
        /// ����Ϊ������־
        /// </summary>
        public static void Save()
        {
            try
            {
                File.AppendAllText(Path.Combine(TxtRecorder.LogPath, $"{Unit}.count.log"), JsonConvert.SerializeObject(Station, Formatting.Indented));
            }
            catch (Exception exception)
            {
                Console.WriteLine(exception);
            }
        }
    }

    /// <summary>
    /// ·�ɼ������ڵ�
    /// </summary>
    [JsonObject(MemberSerialization.OptIn), DataContract]
    internal class CountItem
    {
        /// <summary>
        /// ���ü���ֵ
        /// </summary>
        /// <param name="tm"></param>
        /// <param name="data"></param>
        public void SetValue(double tm, RouteData data)
        {
            LastTime = tm;
            Count += 1;

            if (data.Status == RouteStatus.DenyAccess)
            {
                Deny += 1;
            }
            else if (data.Status >= RouteStatus.LocalError)
            {
                Error += 1;
            }
            else
            {
                if (tm > AppConfig.Config.SystemConfig.WaringTime)
                {
                    TimeOut += 1;
                }
                TotalTime += tm;
                AvgTime = TotalTime / Count;
                if (MaxTime < tm)
                    MaxTime = tm;
                if (MinTime > tm)
                    MinTime = tm;
            }
        }

        /// <summary>
        /// �ʱ��
        /// </summary>
        [DataMember, JsonProperty]
        public double MaxTime { get; set; } = double.MinValue;

        /// <summary>
        /// ���ʱ��
        /// </summary>
        [DataMember, JsonProperty]
        public double MinTime { get; set; } = Double.MaxValue;
        /// <summary>
        /// ��ʱ��
        /// </summary>
        [DataMember, JsonProperty]
        public double TotalTime { get; set; }
        /// <summary>
        /// ƽ��ʱ��
        /// </summary>
        [DataMember, JsonProperty]
        public double AvgTime { get; set; }

        /// <summary>
        /// ���ʱ��
        /// </summary>
        [DataMember, JsonProperty]
        public double LastTime { get; set; }
        
        /// <summary>
        /// �ܴ���
        /// </summary>
        [DataMember, JsonProperty]
        public int Count { get; set; }

        /// <summary>
        /// �������
        /// </summary>
        [DataMember, JsonProperty]
        public int TimeOut { get; set; }

        /// <summary>
        /// �������
        /// </summary>
        [DataMember, JsonProperty]
        public int Error { get; set; }

        /// <summary>
        /// �ܾ�����
        /// </summary>
        [DataMember, JsonProperty]
        public int Deny { get; set; }
        
        /// <summary>
        /// �Ӽ�
        /// </summary>
        [DataMember, JsonProperty]
        public Dictionary<string, CountItem> Items { get; set; } = new Dictionary<string, CountItem>(StringComparer.OrdinalIgnoreCase);
    }
}