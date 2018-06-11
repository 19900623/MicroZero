using System;
using System.Collections.Generic;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.Log;

namespace Agebull.ZeroNet.ZeroApi
{
    /// <summary>
    /// 性能计数器
    /// </summary>
    internal sealed class ApiCounter : BatchPublisher<CountData>, IApiCounter
    {
        private ApiCounter()
        {
            Name = "ApiCounter";
            StationName = "HealthCenter";
            ZeroApplication.RegistZeroObject(this);
        }
        /// <summary>
        /// 实例
        /// </summary>
        public static readonly ApiCounter Instance = new ApiCounter();

        /// <summary>
        /// 数据进入的处理
        /// </summary>
        protected override void OnSend(List<CountData> datas)
        {
            foreach (var  data in datas)
            {
                data.Title = "ApiCounter";
            }
        }

        /// <summary>
        /// 设置Api调用注入
        /// </summary>
        public void HookApi()
        {
            ApiStation.PreActions.Add(Instance.OnPre);
            ApiStation.EndActions.Add(Instance.OnEnd);
            ZeroApplication.RegistZeroObject(Instance);
            _isEnable = true;
        }

        /// <summary>
        /// 统计
        /// </summary>
        /// <param name="data"></param>
        void IApiCounter.Count(CountData data)
        {
            throw new NotImplementedException();
        }

        private readonly Dictionary<string, CountData> _handlers = new Dictionary<string, CountData>();

        /// <summary>
        /// 是否启用
        /// </summary>
        private bool _isEnable;

        /// <summary>
        /// 是否启用
        /// </summary>
        bool IApiCounter.IsEnable => _isEnable;

        private void OnPre(ApiStation station, ApiCallItem item)
        {
            var count = new CountData
            {
                Start = DateTime.Now,
                Machine = ZeroApplication.Config.StationName,
                User = ApiContext.Customer?.Account ?? "Unknow",
                ToId = item.GlobalId,
                RequestId = ApiContext.RequestContext.RequestId,
                Requester = item.Caller,
                HostName = station.StationName,
                ApiName = item.ApiName
            };
            lock (_handlers)
                _handlers.Add(item.GlobalId, count);
        }

        private void OnEnd(ApiStation station,ApiCallItem  item)
        {
            CountData count;
            lock (_handlers)
            {
                if (!_handlers.TryGetValue(item.GlobalId, out count))
                    return;
                _handlers.Remove(item.GlobalId);
            }
            count.End = DateTime.Now;
            count.Status = item.Status;
            count.FromId = item.CallerGlobalId;
            Publish(count);
        }
    }
}