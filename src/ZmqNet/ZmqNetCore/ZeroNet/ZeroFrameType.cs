namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// ֡����
    /// </summary>
    public class ZeroFrameType
    {
        /// <summary>
        /// ��ֹ����
        /// </summary>
        public const byte End = (byte)'\0';
        /// <summary>
        /// ��ֹ����
        /// </summary>
        public const byte GlobalId = (byte)1;
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


        /// <summary>
        /// �㲥����
        /// </summary>
        public const byte PubTitle = (byte) '*';
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