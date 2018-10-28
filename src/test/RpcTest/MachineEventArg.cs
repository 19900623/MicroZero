using Agebull.Common.Rpc;
using Agebull.ZeroNet.ZeroApi;
using Gboxt.Common.DataModel;
using System.Text.RegularExpressions;

namespace ApiTest
{
    /// <summary>
    /// ��¼����
    /// </summary>
    public class MachineEventArg : IApiArgument
    {
        /// <summary>
        /// �¼�����
        /// </summary>
        public string EventName { get; set; }

        /// <summary>
        /// ������ʶ
        /// </summary>
        public string MachineId { get; set; }

        /// <summary>
        /// �¼�����
        /// </summary>
        public string JsonStr { get; set; }
        
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

    public class OpenDoorArg
    {
        public string CompanyId {get;set;}//��˾id
        public string UserType {get;set;}//�û�����
        public string UserId {get;set;}//�û�id
        public string DeviceId {get;set;}//�豸id
        public string RecordDate{get;set;}//��¼ʱ��
        public string RecordUserStatus{get;set;}//״̬
        public string InOrOut{get;set;}//�����
        public string EnterType{get;set;}//������ʽ
        public string PhotoUrl{get;set;}//������
        public string IdentityImageUrl{get;set;}//֤����
        public string PanoramaUrl{get;set;}//ȫ����
        public string Score{get;set;}//ʶ��ϵ��

    }
}
