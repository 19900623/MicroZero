namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// ������Ϊ����
    /// </summary>
    public enum ZeroActionType : sbyte
    {
        /// <summary>
        /// ������˵��
        /// </summary>
        None = 0,
        /// <summary>
        /// ����վ�����
        /// </summary>
        WorkerJoin = 1,
        /// <summary>
        /// ����վ���˳�
        /// </summary>
        WorkerLeft = 2,
        /// <summary>
        /// ����վ��ȴ�����
        /// </summary>
        WrokerListen = 3
    }
    /// <summary>
    /// ��׼״̬
    /// </summary>
    public enum ZeroStateType : sbyte
    {
        /// <summary>
        /// �����ɹ�
        /// </summary>
        Success = 0,
        /// <summary>
        /// �ƻ�ִ��
        /// </summary>
        Plan = 1,
        /// <summary>
        /// ִ����
        /// </summary>
        VoteRuning = 2,
        /// <summary>
        /// ͶƱ���˳�
        /// </summary>
        VoteBye = 3,
        /// <summary>
        /// ͶƱ��ӭ
        /// </summary>
        VoteWecome = 4,
        /// <summary>
        /// ͶƱ�ѷ���
        /// </summary>
        VoteSend = 5,
        /// <summary>
        /// ͶƱ�ȴ�ִ��
        /// </summary>
        VoteWaiting = 6,
        /// <summary>
        /// ͶƱ�ѿ�ʼ
        /// </summary>
        VoteStart = 7,
        /// <summary>
        /// ͶƱ����
        /// </summary>
        VoteEnd = 8,
        /// <summary>
        /// ��������
        /// </summary>
        Error = -1,
        /// <summary>
        /// ����ʧ��
        /// </summary>
        Failed = -2,
        /// <summary>
        /// �Ҳ���վ��
        /// </summary>
        NoFind = -3,
        /// <summary>
        /// ��֧�ֵĲ���
        /// </summary>
        NoSupport = -4,
        /// <summary>
        /// ����У��ʧ��
        /// </summary>
        Invalid = -5,
        /// <summary>
        /// ��ʱʧ��
        /// </summary>
        TimeOut = -6,
        /// <summary>
        /// �������
        /// </summary>
        NetError = -7,
        /// <summary>
        /// û�й�������
        /// </summary>
        NoWorker = -8,
        /// <summary>
        /// ���������ʽ����
        /// </summary>
        CommandArgumentError = -9,
        /// <summary>
        /// ��װ�����ʽ����
        /// </summary>
        InstallArgumentError = -10
    }

    /// <summary>
    /// ֡����
    /// </summary>
    public class ZeroFrameType
    {
        /// <summary>
        /// ��ֹ����
        /// </summary>
        public const byte End = (byte)'E';
        /// <summary>
        /// ִ�мƻ�
        /// </summary>
        public const byte Plan = (byte)'P';
        /// <summary>
        /// ����
        /// </summary>
        public const byte Argument = (byte)'A';
        /// <summary>
        /// ����ID
        /// </summary>
        public const byte RequestId = (byte)'I';
        /// <summary>
        /// ������
        /// </summary>
        public const byte Requester = (byte)'R';
        /// <summary>
        /// ������/������
        /// </summary>
        public const byte Publisher = Requester;
        /// <summary>
        /// �ظ���
        /// </summary>
        public const byte Responser = (byte)'G';
        /// <summary>
        /// ������/�˷���
        /// </summary>
        public const byte Subscriber = Responser;
        //�㲥����
        //#define zero_pub_title  '*'
        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte SubTitle = (byte)'S';
        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte Status = (byte)'S';
        /// <summary>
        /// ������������Ϣ
        /// </summary>
        public const byte Context = (byte)'T';
        /// <summary>
        /// ������������Ϣ
        /// </summary>
        public const byte Command = (byte)'C';
        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte TextValue = (byte)'T';
        /// <summary>
        /// ������������Ϣ
        /// </summary>
        public const byte JsonValue = (byte)'J';
        /// <summary>
        /// ������������Ϣ
        /// </summary>
        public const byte BinaryValue = (byte)'B';
    }
}