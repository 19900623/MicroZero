using Agebull.Common.Ioc;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Options;
using Senparc.CO2NET;
using Senparc.CO2NET.RegisterServices;
using Senparc.Weixin;
using Senparc.Weixin.Cache.Redis;
using Senparc.Weixin.Entities;
using Senparc.Weixin.MP;
using Senparc.Weixin.RegisterServices;

namespace ZeroNet.Http.Gateway
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
            RouteApp.Initialize();
        }
        
        public static IConfiguration Configuration { get; set; }

        //
        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            IocHelper.SetServiceCollection(services);
            services.AddMemoryCache();
            services.AddSenparcGlobalServices(Configuration)//Senparc.CO2NET ȫ��ע��
                .AddSenparcWeixinServices(Configuration);//Senparc.Weixin ע��
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app,IHostingEnvironment env, 
            IOptions<SenparcSetting> senparcSetting, IOptions<SenparcWeixinSetting> senparcWeixinSetting)
        {

            //����EnableRequestRewind�м��
            app.UseEnableRequestRewind();


            // ���� CO2NET ȫ��ע�ᣬ���룡
            IRegisterService register = RegisterService.Start(env, senparcSetting.Value)
                                                        .UseSenparcGlobal();

            //�����Ҫ�Զ�ɨ���Զ�����չ���棬��������ʹ�ã�
            //register.UseSenparcGlobal(true);
            //�����Ҫָ���Զ�����չ���棬���������ã�
            //register.UseSenparcGlobal(false, GetExCacheStrategies);

            #region CO2NET ȫ������

            #region ȫ�ֻ������ã����裩

            //��ͬһ���ֲ�ʽ����ͬʱ�����ڶ����վ��Ӧ�ó���أ�ʱ������ʹ�������ռ佫����루�Ǳ��룩
            register.ChangeDefaultCacheNamespace("DefaultCO2NETCache");

            #region ���ú�ʹ�� Redis 
            Senparc.CO2NET.Cache.Redis.Register.SetConfigurationOption(senparcSetting.Value.Cache_Redis_Configuration);
            Senparc.CO2NET.Cache.Redis.Register.UseKeyValueRedisNow();//��ֵ�Ի�����ԣ��Ƽ���
            app.UseSenparcWeixinCacheRedis();
            #endregion                        // PDBMARK_END

            #endregion

            #endregion

            #region ΢���������



            //��ʼע��΢����Ϣ�����룡
            register.UseSenparcWeixin(senparcWeixinSetting.Value, senparcSetting.Value)
                //ע�ṫ�ںţ���ע������                                                -- PDBMARK MP
                .RegisterMpAccount(senparcWeixinSetting.Value, "���ں�");// PDBMARK_END


            #endregion
            app.Run(RouteApp.Call);
        }

    }
}