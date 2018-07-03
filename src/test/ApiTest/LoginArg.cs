using Agebull.ZeroNet.ZeroApi;

namespace ApiTest
{
    /// <summary>
    /// ��¼����
    /// </summary>
    public class LoginArg : IApiArgument
    {
        /// <summary>
        /// �ֻ���
        /// </summary>
        /// <value>11λ�ֻ���,����Ϊ��</value>
        /// <example>15618965007</example>
        public string MobilePhone { get; set; }
        /// <summary>
        /// ����
        /// </summary>
        /// <value>6-16λ�����ַ�\��ĸ\�������,�����ַ�\��ĸ\���ֶ���Ҫһ����,����Ϊ��</value>
        /// <example>pwd#123</example>
        public string UserPassword { get; set; }
        /// <summary>
        /// ��֤��
        /// </summary>
        /// <value>6λ��ĸ������,����Ϊ��</value>
        /// <example>123ABC</example>
        public string VerificationCode { get; set; }
        
        /// <summary>
        /// У��
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        bool IApiArgument.Validate(out string message)
        {
            message = null;
            return true;
        }
    }
}
