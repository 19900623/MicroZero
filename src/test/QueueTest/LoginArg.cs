using Gboxt.Common.DataModel;

namespace ApiTest
{ 
    /// <summary>
    /// ��¼����
    /// </summary>
    public class TestArg : IApiArgument
    {
        /// <summary>
        /// �ֻ���
        /// </summary>
        /// <value>11λ�ֻ���,����Ϊ��</value>
        /// <example>15618965007</example>
        [DataRule(CanNull = false, Max = 11, Min = 11, Regex = "1[3-9]\\d{9,9}")]
        public string MobilePhone { get; set; }

        /// <summary>
        /// ����
        /// </summary>
        /// <value>6-16λ�����ַ�\��ĸ\�������,�����ַ�\��ĸ\���ֶ���Ҫһ����,����Ϊ��</value>
        /// <example>pwd#123</example>
        [DataRule(CanNull = false, Max = 6, Min = 16, Regex = "[\\da-zA-Z~!@#$%^&*]{6,16}")]
        public string UserPassword { get; set; }
        /// <summary>
        /// ��֤��
        /// </summary>
        /// <value>6λ��ĸ������,����Ϊ��</value>
        /// <example>123ABC</example>
        [DataRule(CanNull = false, Max = 6, Min = 6, Regex = "[a-zA-Z\\d]{6,6}")]
        public string VerificationCode { get; set; }
        
        /// <summary>
        /// ״̬
        /// </summary>
        public int State { get; set; }

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
