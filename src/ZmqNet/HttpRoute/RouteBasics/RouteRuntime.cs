using Agebull.ZeroNet.ZeroApi;
using Newtonsoft.Json;

namespace ZeroNet.Http.Route
{
    /// <summary>
    ///     ·������ʱ������ص�����
    /// </summary>
    public class RouteRuntime
    {
        /// <summary>
        ///     �ܾ����ʵ�Json�ַ���
        /// </summary>
        public static readonly string DenyAccess = JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.DenyAccess, "*�ܾ�����*"));
        
        /// <summary>
        ///     �������޷���ֵ���ַ���
        /// </summary>
        public static readonly string RemoteEmptyError =
            JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.UnknowError, "*�������޷���ֵ*"));

        /// <summary>
        ///     �����������쳣
        /// </summary>
        public static readonly string NetworkError =
            JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.NetworkError, "*�����������쳣*"));

        /// <summary>
        ///     �����������쳣
        /// </summary>
        public static readonly string Inner2Error =
            JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.InnerError, "**ϵͳ�ڲ�����**"));

        /// <summary>
        ///     �����������쳣
        /// </summary>
        public static readonly string InnerError =
            JsonConvert.SerializeObject(ApiResult.Error(ErrorCode.InnerError, "*ϵͳ�ڲ�����*"));
    }
    
}