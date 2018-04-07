using System;
using System.Collections.Generic;
using Agebull.Common.Logging;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.PubSub;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ����ʱ����
    /// </summary>
    public class RuntimeWaring
    {
        /// <summary>
        ///     ˢ��
        /// </summary>
        public static void Flush()
        {
            ZeroPublisher.Publish("HealthCenter", "RuntimeWaring", "Flush",null);
        }
        
        /// <summary>
        /// ����ʱ����
        /// </summary>
        /// <param name="host"></param>
        /// <param name="api"></param>
        /// <param name="message"></param>
        public static void Waring(string host, string api, string message)
        {
            if (AppConfig.Config.SmsConfig == null || AppConfig.Config.SmsConfig.CycleHours <= 0)
                return;
            ZeroPublisher.Publish("HealthCenter", "RuntimeWaring", "Waring", JsonConvert.SerializeObject(
            new{
                host,
                api,
                message
            }));
        }
        
    }

}