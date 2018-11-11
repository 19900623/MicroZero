namespace Agebull.ZeroNet.Core
{
    /// <summary>
    ///     ֡����
    /// </summary>
    public class ZeroFrameType
    {
        /// <summary>
        ///     ��ֹ����
        /// </summary>
        public const byte End = 0;

        /// <summary>
        ///     ȫ�ֱ�ʶ
        /// </summary>
        public const byte GlobalId = 1;

        /// <summary>
        ///     վ��ID
        /// </summary>
        public const byte StationId = 8;

        /// <summary>
        ///     վ������֡
        /// </summary>
        public const byte Station = 2;

        /// <summary>
        ///     ״̬֡
        /// </summary>
        public const byte Status = 3;

        /// <summary>
        ///     ����ID
        /// </summary>
        public const byte RequestId = 4;

        /// <summary>
        ///     ִ�мƻ�
        /// </summary>
        public const byte Plan = 5;

        /// <summary>
        ///     �ƻ�ʱ��
        /// </summary>
        public const byte PlanTime = 6;

        /// <summary>
        ///     ������֤��ʶ
        /// </summary>
        public const byte SerivceKey = 7;

        /// <summary>
        ///     ����
        /// </summary>
        public const byte Argument = (byte)'%';

        /// <summary>
        ///     ������
        /// </summary>
        public const byte Requester = (byte)'>';

        /// <summary>
        ///     ������/������
        /// </summary>
        public const byte Publisher = Requester;

        /// <summary>
        ///     �ظ���
        /// </summary>
        public const byte Responser = (byte)'<';

        /// <summary>
        ///     ������/�˷���
        /// </summary>
        public const byte Subscriber = Responser;

        /// <summary>
        ///     ������������Ϣ
        /// </summary>
        public const byte Context = (byte)'#';

        /// <summary>
        ///     ��������
        /// </summary>
        public const byte Command = (byte)'$';

        /// <summary>
        ///     �㲥����
        /// </summary>
        public const byte PubTitle = (byte)'*';

        /// <summary>
        ///     �㲥����
        /// </summary>
        public const byte SubTitle = Command;


        /// <summary>
        ///     һ���ı�����
        /// </summary>
        public const byte Content = (byte)'T';

        /// <summary>
        ///     JSON�ı�����
        /// </summary>
        public const byte JsonValue = (byte)'J';

        /// <summary>
        ///     ����������
        /// </summary>
        public const byte BinaryValue = (byte)'B';

        /// <summary>
        ///     ����������
        /// </summary>
        public const byte TsonValue = (byte)'V';

        /// <summary>
        ///     ˵��֡����
        /// </summary>
        public static string FrameName(byte value)
        {
            switch (value)
            {
                case End:// 0;
                    return @"��ֹ����";
                case GlobalId:// 1;
                    return @"ȫ�ֱ�ʶ";
                case Station:// 2;
                    return @"վ������֡";
                case Status:// 3;
                    return @"״̬֡";
                case RequestId:// 4;
                    return @"����ID";
                case Plan:// 5;
                    return @"ִ�мƻ�";
                case SerivceKey:// 5;
                    return @"������֤��ʶ"; 
                case PlanTime:// 5;
                    return @"�ƻ�ʱ��"; 
                case Argument:// (byte)'%';
                    return @"����";
                case Requester:// (byte)'>';
                    return @"������/������/������";
                case Responser:// (byte)'<';
                    return @"�ظ���/������/�˷���";
                case PubTitle:// (byte)'*';
                    return @"�㲥����";
                case Context:// (byte)'#';
                    return @"������������Ϣ";
                case Command:// (byte)'$';
                    return @"��������";
                case Content:// (byte)'T';
                    return @"һ���ı�����";
                case JsonValue:// (byte)'J';
                    return @"JSON�ı�����";
                case BinaryValue:// (byte)'B';
                    return @"����������";
                default:
                    return "Error";
            }
        }
    }
}