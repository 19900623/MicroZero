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
        public string Caller => ApiContext.RequestContext.ServiceKey;
        /// <summary>
        /// ���ñ�ʶ
        /// </summary>
        public string RequestId => ApiContext.RequestContext.RequestId;
        /// <summary>
        /// HTTP����ʱ��UserAgent
        /// </summary>
        public string UserAgent => ApiContext.RequestContext.UserAgent;
        
    }
}