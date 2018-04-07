using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using Agebull.Common.Logging;
using Agebull.ZeroNet.PubSub;
using Aliyun.Acs.Dysmsapi.Model.V20170525;
using Aliyun.Net.SDK.Core;
using Aliyun.Net.SDK.Core.Exceptions;
using Aliyun.Net.SDK.Core.Profile;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    /// ����ʱ����
    /// </summary>
    public class RuntimeWaring : SubStation
    {
        /// <summary>
        /// ����
        /// </summary>
        public RuntimeWaring()
        {
            StationName = "HealthCenter";
            Subscribe = "RuntimeWaring";
        }
        /// <summary>
        /// ִ������
        /// </summary>
        /// <param name="args"></param>
        /// <returns></returns>
        public override void Handle(PublishItem args)
        {
            if (args.SubTitle == "Flush")
            {
                Flush();
                return;
            }
            try
            {
                var data = JsonConvert.DeserializeObject<WaringItem>(args.Content);
                Waring(data.Host ,data.Api,data.Message);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
        /// <summary>
        ///     ˢ��
        /// </summary>
        internal static void Flush()
        {
            lock (WaringsTime)
                WaringsTime.Clear();
        }


        /// <summary>
        /// ��������ʱ����
        /// </summary>
        internal static readonly Dictionary<string, RuntimeWaringItem> WaringsTime = new Dictionary<string, RuntimeWaringItem>(StringComparer.OrdinalIgnoreCase);

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
            RuntimeWaringItem item;
            lock (WaringsTime)
            {
                if (!WaringsTime.TryGetValue(host, out item))
                {
                    WaringsTime.Add(host, item = new RuntimeWaringItem
                    {
                        StartTime = DateTime.Now,
                        LastTime = DateTime.Now,
                        WaringCount = 1,
                        LastCount = 1
                    });
                }
                else
                {
                    item.LastTime = DateTime.Now;
                    if (item.MessageTime != DateTime.MinValue && (DateTime.Now - item.MessageTime).TotalHours > AppConfig.Config.SmsConfig.CycleHours)
                    {
                        item.SendCount = 0;
                        item.WaringCount = 1;
                        item.LastCount = 1;
                    }
                    else
                    {
                        item.WaringCount += 1;
                        item.LastCount += 1;
                    }
                }

                if (!item.Apis.ContainsKey(api))
                    item.Apis.Add(api, new List<string> { message });
                else if (!item.Apis[api].Contains(message))
                    item.Apis[api].Add(message);
                //�ѵ���෢��������ֵ
                if (item.SendCount > AppConfig.Config.SmsConfig.CycleSendCount)
                    return;
                //����Ƶ������
                if (item.SendCount > 0)
                {
                    if (item.LastCount <= 10)
                    {
                        if ((DateTime.Now - item.MessageTime).TotalMinutes < 30.0)
                            return;
                    }
                    else if (item.LastCount <= 50)
                    {
                        if ((DateTime.Now - item.MessageTime).TotalMinutes < 20.0)
                            return;
                    }
                    else if (item.LastCount <= 200)
                    {
                        if ((DateTime.Now - item.MessageTime).TotalMinutes < 10.0)
                            return;
                    }
                    else if ((DateTime.Now - item.MessageTime).TotalMinutes < 5.0)
                    {
                        return;
                    }
                }
            }
            if (api.Length >= 20)
                api = api.Substring(api.Length - 19, 19);
            if (message.Length >= 20)
                message = message.Substring(20);
            //���Ͷ���
            Console.WriteLine($"������{host}��{api}����${message}����{item.LastCount}�Σ�����������");
            foreach (var phone in AppConfig.Config.SmsConfig.Phones)
            {
                if (!SendSmsByAli(host, phone, api, message, item.WaringCount))
                    continue;
                item.MessageTime = DateTime.Now;
                item.SendCount++;
                item.LastCount = 0;
            }
        }

        /// <summary>
        /// ������ŷ���
        /// </summary>
        /// <param name="server"></param>
        /// <param name="phoneNumber"></param>
        /// <param name="api"></param>
        /// <param name="message"></param>
        /// <param name="count"></param>
        /// <returns></returns>
        private static bool SendSmsByAli(string server, string phoneNumber, string api, string message, int count)
        {
            IClientProfile profile = DefaultProfile.GetProfile(AppConfig.Config.SmsConfig.AliRegionId, AppConfig.Config.SmsConfig.AliAccessKeyId, AppConfig.Config.SmsConfig.AliAccessKeySecret);
            DefaultProfile.AddEndpoint(AppConfig.Config.SmsConfig.AliEndPointName, AppConfig.Config.SmsConfig.AliRegionId, AppConfig.Config.SmsConfig.AliProduct, AppConfig.Config.SmsConfig.AliDomain);
            IAcsClient acsClient = new DefaultAcsClient(profile);
            var request = new SendSmsRequest
            {
                PhoneNumbers = phoneNumber,
                SignName = AppConfig.Config.SmsConfig.AliSignName,
                TemplateCode = AppConfig.Config.SmsConfig.AliTemplateCode,
                //������${server}��${url}����${message}����${count}�Σ�����������
                TemplateParam = JsonConvert.SerializeObject(new
                {
                    server,
                    api,
                    message,
                    count
                }),
                OutId = server
            };
            SendSmsResponse sendSmsResponse;
            try
            {
                //����ʧ���������ClientException�쳣
                sendSmsResponse = acsClient.GetAcsResponse(request);
            }
            catch (ServerException e)
            {
                LogRecorder.Exception(e);
                return false;
            }
            catch (ClientException e)
            {
                LogRecorder.Exception(e);
                return false;
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return false;
            }
            return sendSmsResponse.Message == "OK";
        }

    }

    /// <summary>
    /// ����ʱ����ڵ�
    /// </summary>
    public class RuntimeWaringItem
    {
        /*��ֹʱ��*/
        public DateTime StartTime, LastTime;
        /// <summary>
        /// ���һ�η�����ʱ��
        /// </summary>
        public DateTime MessageTime;
        /// <summary>
        /// �������������ʹ��������ͺ�������
        /// </summary>
        public int WaringCount, SendCount, LastCount;
        /// <summary>
        /// ���������API
        /// </summary>
        public Dictionary<string, List<string>> Apis = new Dictionary<string, List<string>>();
    }

    [JsonObject(MemberSerialization.OptIn), DataContract]
    public class WaringItem
    {
        [DataMember, JsonProperty("host")] public string Host;
        [DataMember, JsonProperty("api")]
        public string Api;
        [DataMember, JsonProperty("message")]
        public string Message;
    }
}