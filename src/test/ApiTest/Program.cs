using System.Reflection;
using Agebull.Common.Configuration;
using Agebull.Common.Ioc;
using Agebull.MicroZero;
using Agebull.MicroZero.ZeroApis;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Senparc.CO2NET;
using Senparc.CO2NET.RegisterServices;
using Senparc.Weixin;
using Senparc.Weixin.Entities;
using Senparc.Weixin.MP.Containers;
using Senparc.Weixin.RegisterServices;



namespace ApiTest
{
    partial class Program
    {


        static void Main(string[] args)
        {
            ZeroApplication.CheckOption();
            ZeroApplication.Discove(Assembly.GetExecutingAssembly());
            ZeroApplication.Initialize();

            ZeroApplication.RunAwaite();
        }

        //static void Weixin()
        //{
        //    IocHelper.ServiceCollection.AddMemoryCache();

        //    IocHelper.ServiceCollection
        //        .AddSenparcGlobalServices(ConfigurationManager.Root)//Senparc.CO2NET ȫ��ע��
        //        .AddSenparcWeixinServices(ConfigurationManager.Root);//Senparc.Weixin ע��

        //    var senparcSetting = ConfigurationManager.Root.GetSection("SecurityHeaderOptions").Get<SenparcSetting>();
        //    var senparcWeixinSetting = ConfigurationManager.Root.GetSection("SenparcWeixinSetting").Get<SenparcWeixinSetting>();
        //    RegisterService.Start(null, senparcSetting)
        //        .UseSenparcGlobal()// ���� CO2NET ȫ��ע��
        //        .UseSenparcWeixin(senparcWeixinSetting, senparcSetting);//΢��ȫ��ע��
        //    //ע��AppId
        //    AccessTokenContainer.Register(senparcWeixinSetting.WeixinAppId, senparcWeixinSetting.WeixinAppSecret);
        //}
    }

    /// <summary>
    /// Weixin����
    /// </summary>
    [ApiPage("/index.htm")]
    public class TestController : ApiController
    {
        /// <summary>
        /// ������������
        /// </summary>
        /// <returns></returns>
        [Route("v1/msg/text")]
        public ApiResult  OnTextRequest(Argument a)
        {
            return ApiResult.ArgumentError ;
        }
    }
}
