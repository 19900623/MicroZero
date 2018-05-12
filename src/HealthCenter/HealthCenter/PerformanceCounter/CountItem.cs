using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
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