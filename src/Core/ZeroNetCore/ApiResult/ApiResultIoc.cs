using Agebull.Common.Ioc;
using Newtonsoft.Json;

namespace Agebull.MicroZero.ZeroApis
{
    /// <summary>API���ػ���</summary>
    public static class ApiResultIoc
    {
        /// <summary>
        /// ApiResult�ĳ���
        /// </summary>
        private static IApiResultDefault _ioc;

        /// <summary>
        /// ApiResult�ĳ���
        /// </summary>
        public static IApiResultDefault Ioc =>
            _ioc ?? (_ioc = IocHelper.Create<IApiResultDefault>() ?? new ApiResultDefault());
        


        /// <summary>�ɹ���Json�ַ���</summary>
        /// <remarks>�ɹ�</remarks>
        public static string SucceesJson => JsonConvert.SerializeObject(Ioc.Ok);

        /// <summary>ҳ�治���ڵ�Json�ַ���</summary>
        public static string NoFindJson => JsonConvert.SerializeObject(Ioc.NoFind);

        /// <summary>ϵͳ��֧�ֵ�Json�ַ���</summary>
        public static string NotSupportJson => JsonConvert.SerializeObject(Ioc.NotSupport);

        /// <summary>���������ַ���</summary>
        public static string ArgumentErrorJson => JsonConvert.SerializeObject(Ioc.ArgumentError);

        /// <summary>�߼������ַ���</summary>
        public static string LogicalErrorJson => JsonConvert.SerializeObject(Ioc.LogicalError);

        /// <summary>�ܾ����ʵ�Json�ַ���</summary>
        public static string DenyAccessJson => JsonConvert.SerializeObject(Ioc.DenyAccess);

        /// <summary>�������޷���ֵ���ַ���</summary>
        public static string RemoteEmptyErrorJson => JsonConvert.SerializeObject(Ioc.RemoteEmptyError);

        /// <summary>�����������쳣</summary>
        public static string NetworkErrorJson => JsonConvert.SerializeObject(Ioc.NetworkError);

        /// <summary>���ش���</summary>
        public static string LocalErrorJson => JsonConvert.SerializeObject(Ioc.LocalError);

        /// <summary>���ط����쳣��Json�ַ���</summary>
        public static string LocalExceptionJson => JsonConvert.SerializeObject(Ioc.LocalException);

        /// <summary>ϵͳδ������Json�ַ���</summary>
        public static string NoReadyJson => JsonConvert.SerializeObject(Ioc.NoReady);

        /// <summary>��ͣ�����Json�ַ���</summary>
        public static string PauseJson => JsonConvert.SerializeObject(Ioc.Pause);

        /// <summary>δ֪�����Json�ַ���</summary>
        public static string UnknowErrorJson => JsonConvert.SerializeObject(Ioc.UnknowError);

        /// <summary>���糬ʱ��Json�ַ���</summary>
        /// <remarks>��������Apiʱʱ�׳�δ�����쳣</remarks>
        public static string TimeOutJson => JsonConvert.SerializeObject(Ioc.TimeOut);

        /// <summary>�ڲ������Json�ַ���</summary>
        /// <remarks>ִ�з���ʱ�׳�δ�����쳣</remarks>
        public static string InnerErrorJson => JsonConvert.SerializeObject(Ioc.InnerError);

        /// <summary>���񲻿��õ�Json�ַ���</summary>
        public static string UnavailableJson => JsonConvert.SerializeObject(Ioc.Unavailable);
    }
}