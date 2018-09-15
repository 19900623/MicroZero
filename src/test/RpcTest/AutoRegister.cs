using System.ComponentModel.Composition;
using Agebull.Common.Ioc;
using Agebull.ZeroNet.Core;
using Agebull.ZeroNet.ZeroApi;
using Gboxt.Common.DataModel.ZeroNet;
using Microsoft.Extensions.DependencyInjection;
using RpcTest;

namespace Agebull.ZeroNet.Log
{
    /// <summary>
    ///   远程记录器
    /// </summary>
    [Export(typeof(IAutoRegister))]
    [ExportMetadata("Symbol", '%')]
    public sealed class AutoRegister : IAutoRegister
    { 
        /// <summary>
        /// 初始化
        /// </summary>
        void IAutoRegister.Initialize()
        {
            IocHelper.ServiceCollection.AddSingleton<Tester, ZeroPublishTester>();
        }

        /// <summary>
        /// 注册
        /// </summary>
        void IAutoRegister.AutoRegist()
        {
            ZeroApplication.ZeroNetEvent += Tester.OnZeroEvent;
            ZeroApplication.RegistZeroObject(TestEventProxy.Instance);
        }
    }
}