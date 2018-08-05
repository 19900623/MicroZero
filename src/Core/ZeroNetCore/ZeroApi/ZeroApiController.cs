using Agebull.Common.OAuth;

namespace Agebull.ZeroNet.ZeroApi
{
    /// <summary>
    /// ZeroApi����������
    /// </summary>
    public class ZeroApiController
    {
        /// <summary>
        /// ��ǰ��¼�û�
        /// </summary>
        public ILoginUserInfo UserInfo => ApiContext.Customer;
        /// <summary>
        /// �����ߣ���������
        /// </summary>
        public string Caller => ApiContext.RequestInfo.ServiceKey;
        /// <summary>
        /// ���ñ�ʶ
        /// </summary>
        public string RequestId => ApiContext.RequestInfo.RequestId;
        /// <summary>
        /// HTTP����ʱ��UserAgent
        /// </summary>
        public string UserAgent => ApiContext.RequestInfo.UserAgent;
        
    }
}