using System;
using Agebull.ZeroNet.PubSub;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ���ܼ�����
    /// </summary>
    public class PerformanceCounter
    {
        /// <summary>
        /// ��ʼʱ��
        /// </summary>
        public DateTime Start { get; set; }
        
        /// <summary>
        /// ��ʼ����
        /// </summary>
        /// <returns></returns>
        public static PerformanceCounter OnBegin(RouteData data)
        {
            data.Start = DateTime.Now;
            return new PerformanceCounter();
        }

        /// <summary>
        /// ��ʼ����
        /// </summary>
        /// <returns></returns>
        public void End(RouteData data)
        {
            data.End = DateTime.Now;
            ZeroPublisher.Publish("HealthCenter", "PerformanceCounter", "Counter", JsonConvert.SerializeObject(data));
        }


        /// <summary>
        /// ����Ϊ������־
        /// </summary>
        public static void Save()
        {
            ZeroPublisher.Publish("HealthCenter", "PerformanceCounter", "Save",null);
        }
    }
}