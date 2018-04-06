using System;
using Agebull.ZeroNet.Core;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ·�ɼ�����
    /// </summary>
    public class RouteCounter
    {
        /// <summary>
        /// ��ʼʱ��
        /// </summary>
        public DateTime Start { get; set; }
        
        /// <summary>
        /// ��ʼ����
        /// </summary>
        /// <returns></returns>
        public static RouteCounter OnBegin(RouteData data)
        {
            data.Start = DateTime.Now;
            return new RouteCounter();
        }

        /// <summary>
        /// ��ʼ����
        /// </summary>
        /// <returns></returns>
        public void End(RouteData data)
        {
            data.End = DateTime.Now;
            Publisher.Publish("PerformanceCounter", "RouteCounter", JsonConvert.SerializeObject(data));
        }


        /// <summary>
        /// ����Ϊ������־
        /// </summary>
        public static void Save()
        {
            Publisher.Publish("PerformanceCounter", "RouteCounter", "*Save");
        }
    }
}