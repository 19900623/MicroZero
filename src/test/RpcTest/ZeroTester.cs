using System;
using System.Globalization;
using System.Threading;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.ZeroApi;
using ApiTest;
using Newtonsoft.Json;

namespace RpcTest
{
    internal class ZeroTester : Tester
    {
        public override bool Init()
        {
            return SystemManager.Instance.TryInstall(Station, "api");
        }

        private Random random = new Random((int) (DateTime.Now.Ticks % int.MaxValue));
        protected override void DoAsync()
        {
            var arg = new MachineEventArg
            {
                EventName = "OpenDoor",
                MachineId = $"Machine-{random.Next()}",
                JsonStr = JsonConvert.SerializeObject(new OpenDoorArg
                {
                    CompanyId ="��˾id",
                    UserType ="�û�����",
                    UserId = random.Next().ToString(),
                    DeviceId ="�豸id",
                    RecordDate=DateTime.Now.ToString("yyyy-MM-dd"),
                    RecordUserStatus="״̬",
                    InOrOut= $"{((random.Next() % 2) == 1 ? "��" : "��")}",
                    EnterType="������ʽ",
                    PhotoUrl="������",
                    IdentityImageUrl="֤����",
                    PanoramaUrl="ȫ����",
                    Score="ʶ��ϵ��"
                })
            };
            ApiClient client = new ApiClient
            {
                Station = Station,
                Commmand = Api,
                Argument = JsonConvert.SerializeObject(arg)
            };
            client.CallCommand();
            if (client.State < ZeroOperatorStateType.Failed)
            {
            }
            else if (client.State < ZeroOperatorStateType.Error)
            {
                Interlocked.Increment(ref BlError);
            }
            else if (client.State < ZeroOperatorStateType.TimeOut)
            {
                Interlocked.Increment(ref WkError);
            }
            else if (client.State > ZeroOperatorStateType.LocalNoReady)
            {
                Interlocked.Increment(ref ExError);
            }
            else
            {
                Interlocked.Increment(ref NetError);
            }
        }

    }
}