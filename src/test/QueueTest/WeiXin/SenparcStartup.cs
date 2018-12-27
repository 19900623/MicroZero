using System.Collections.Generic;
using Agebull.Common.Configuration;
using Agebull.Common.Ioc;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Senparc.CO2NET;
using Senparc.CO2NET.Cache;
using Senparc.CO2NET.RegisterServices;
using Senparc.Weixin;
using Senparc.Weixin.Cache;
using Senparc.Weixin.Cache.Redis;
using Senparc.Weixin.Entities;
using Senparc.Weixin.MP;
using Senparc.Weixin.RegisterServices;

namespace ZeroNet.Http.Gateway
{
    public class SenparcStartup 
    {
        public IConfiguration Configuration { get; set; }

        
        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddMemoryCache();
            services.AddSenparcGlobalServices(Configuration)//Senparc.CO2NET ȫ��ע��
                .AddSenparcWeixinServices(Configuration);//Senparc.Weixin ע��

            var senparcSetting = ConfigurationManager.Root.GetSection("SenparcSetting")?.Get<SenparcSetting>();
            var senparcWeixinSetting = ConfigurationManager.Root.GetSection("Weixin")?.Get<SenparcWeixinSetting>();

            IRegisterService register = RegisterService.Start(null, senparcSetting).UseSenparcGlobal(false, GetExCacheStrategies);

            //��ʼע��΢����Ϣ�����룡
            register.UseSenparcWeixin(senparcWeixinSetting, senparcSetting).RegisterMpAccount(senparcWeixinSetting);

        }
        /// <summary>
        /// ��ȡ��չ�������
        /// </summary>
        /// <returns></returns>
        private IList<IDomainExtensionCacheStrategy> GetExCacheStrategies()
        {
            return new List<IDomainExtensionCacheStrategy> {LocalContainerCacheStrategy.Instance};
        }
    }
}