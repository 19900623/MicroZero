using System;
using Gboxt.Common.DataModel;
using Newtonsoft.Json;
using System.Runtime.Serialization;
using Agebull.ZeroNet.ZeroApi;

namespace Agebull.Common.OAuth
{
    /// <summary>
    /// ��ǰ��¼���û���Ϣ
    /// </summary>
    public interface IUser : IStateData
    {
        /// <summary>
        /// �û����ֱ�ʶ
        /// </summary>
        long UserId { get; set; }
    }

    /// <summary>
    /// ��ǰ��¼���û���Ϣ
    /// </summary>
    public interface IPerson : IUser
    {
        /// <summary>
        /// �û��ǳ�
        /// </summary>
        string NickName { get; set; }

        /// <summary>
        /// ͷ��
        /// </summary>
        /// <remarks>
        /// ͷ��
        /// </remarks>
        string AvatarUrl
        {
            get;
            set;
        }

    }

    /// <summary>
    /// ��ǰ��¼���û���Ϣ
    /// </summary>
    public interface ILoginUserInfo : IPerson, IApiResultData
    {
        /// <summary>
        /// ��ǰ�û���¼���ĸ�ϵͳ��Ԥ�ȶ����ϵͳ��ʶ��
        /// </summary>
        string LoginSystem { get; set; }

        /// <summary>
        /// ��ǰ�û���¼��ʽ
        /// </summary>
        int LoginType { get; set; }

        /// <summary>
        /// ��¼�ߵ��ֻ���
        /// </summary>
        string Phone { get; set; }

        /// <summary>
        /// ��¼�ߵ��˺�
        /// </summary>
        string Account { get; set; }

        /// <summary>
        /// ��¼�豸�ı�ʶ
        /// </summary>
        string DeviceId { get; set; }

        /// <summary>
        /// ��¼�豸�Ĳ���ϵͳ
        /// </summary>
        string Os { get; set; }

        /// <summary>
        /// ��¼�豸�������
        /// </summary>
        string Browser { get; set; }
    }

    /// <summary>
    /// ��ǰ��¼���û���Ϣ
    /// </summary>
    [Serializable]
    [DataContract, JsonObject(MemberSerialization.OptIn)]
    public class LoginUserInfo : ILoginUserInfo
    {
        /// <summary>
        /// �û����ֱ�ʶ
        /// </summary>
        [JsonProperty("id")]
        public long UserId { get; set; }

        /// <summary>
        /// �û��ǳ�
        /// </summary>
        [JsonProperty("nn")]
        public string NickName { get; set; }

        /// <summary>
        /// ͷ��
        /// </summary>
        /// <remarks>
        /// ͷ��
        /// </remarks>
        [JsonProperty("id")]
        public string AvatarUrl
        {
            get;
            set;
        }
        /// <summary>
        /// ��ǰ�û���¼���ĸ�ϵͳ��Ԥ�ȶ����ϵͳ��ʶ��
        /// </summary>
        [JsonProperty("ls")]
        public string LoginSystem { get; set; }

        /// <summary>
        /// ��ǰ�û���¼��ʽ
        /// </summary>
        [JsonProperty("lt")]
        public int LoginType { get; set; }

        /// <summary>
        /// ��¼�ߵ��ֻ���
        /// </summary>
        [JsonProperty("p")]
        public string Phone { get; set; }


        /// <summary>
        /// ��¼�ߵ��˺�
        /// </summary>
        [JsonProperty("a")]
        public string Account { get; set; }

        /// <summary>
        /// ��¼�豸�ı�ʶ
        /// </summary>
        [JsonProperty("d")]
        public string DeviceId { get; set; }

        /// <summary>
        /// ��¼�豸�Ĳ���ϵͳ
        /// </summary>
        [JsonProperty("os")]
        public string Os { get; set; }

        /// <summary>
        /// ��¼�豸�������
        /// </summary>
        [JsonProperty("br")]
        public string Browser { get; set; }

        /// <summary>
        ///     ����״̬
        /// </summary>
        [JsonProperty("ds")]
        public DataStateType DataState { get; set; }

        /// <summary>
        ///     �����Ƿ��Ѷ��ᣬ����ǣ���Ϊֻ������
        /// </summary>
        /// <value>bool</value>
        [JsonProperty("df")]
        public bool IsFreeze { get; set; }

        #region Ԥ����

        /// <summary>
        /// �����û�
        /// </summary>
        public static LoginUserInfo CreateAnymouse(string did, string br, string os)
        {
            return new LoginUserInfo
            {
                UserId = -2,
                Account = "anymouse",
                Browser = br,
                Os = os,
                DataState = DataStateType.Discard
            };
        }

        /// <summary>
        /// ϵͳ�û�
        /// </summary>
        private static readonly LoginUserInfo _system = new LoginUserInfo
        {
            UserId = -1,
            Account = "system",
            DeviceId = "***system***",
            Browser = "sys",
            Os = "sys",
            IsFreeze = true,
            LoginType = 0,
            DataState = DataStateType.Enable
        };
        /// <summary>
        /// ϵͳ�û�
        /// </summary>
        public static LoginUserInfo System => _system;

        #endregion

    }
}