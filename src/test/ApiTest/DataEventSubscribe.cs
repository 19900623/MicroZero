using System;
using System.Threading;
using System.Timers;
using Agebull.Common.Logging;
using Agebull.Common.Rpc;
using Agebull.ZeroNet.PubSub;
using Gboxt.Common.DataModel.ExtendEvents;
using Newtonsoft.Json;
using Timer = System.Threading.Timer;

namespace ApiTest
{
    /// <summary>
    /// ָ����Ϣ����
    /// </summary>
    public class DataEventSubscribe : SubStation<PublishItem>
    {
        /// <summary>
        /// ����
        /// </summary>
        public DataEventSubscribe()
        {
            Name = "EntityEvent";
            StationName = "EntityEvent";
            IsRealModel = true;
            Subscribe = "";
        }


        /// <summary>
        /// ���ϴδ�����Ƿ������ݸ��£����ڶ�ʱ�������Ƿ�������
        /// </summary>
        private int _eventCount;

        /// <inheritdoc />
        /// <summary>ִ������</summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public override void Handle(PublishItem item)
        {
            if (item == null)
                return;
            EntityEventArgument argument;
            try
            {
                argument = JsonConvert.DeserializeObject<EntityEventArgument>(item.Content);
            }
            catch (Exception ex)
            {
                LogRecorder.Exception(ex);
                return;
            }

            if (argument.ValueType != EntityEventValueType.EntityJson)
                return;
            Interlocked.Increment(ref _eventCount);
        }

    }
}