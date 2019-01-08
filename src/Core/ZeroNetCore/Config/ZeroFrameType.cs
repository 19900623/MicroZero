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
        ///     ��ֹ����
        /// </summary>
        public const byte ExtendEnd = 0xFF;
        /// <summary>
        ///     ��ֹ����
        /// </summary>
        public const byte ResultEnd = 0xFE;

        /// <summary>
        ///     ȫ�ֱ�ʶ
        /// </summary>
        public const byte GlobalId = 1;

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
        ///     ������֤��ʶ
        /// </summary>
        public const byte LocalId = 8;

        /// <summary>
        /// ���÷���վ������
        /// </summary>
        public const byte   StationType = 9;

        /// <summary>
        /// ���÷���ȫ�ֱ�ʶ
        /// </summary>
        public const byte   CallId = 0xB;
        /// <summary>
        /// ���ݷ���
        /// </summary>
        public const byte   DataDirection = 0xC;

        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original1 = 0x10;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original2 = 0x11;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original3 = 0x12;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original4 = 0x13;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original5 = 0x14;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original6 = 0x15;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original7 = 0x16;
        /// <summary>
        /// ԭ������
        /// </summary>
        public const byte Original8 = 0x17;
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
        public const byte TextContent = (byte)'T';

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
                    return @"������ֹ����";
                case ExtendEnd:// 0;
                    return @"������ֹ����";
                case ResultEnd:// 0;
                    return @"������ֹ����";
                case GlobalId:// 1;
                    return @"ȫ�ֱ�ʶ";
                case Station:// 2;
                    return @"վ������֡";
                case Status:// 3;
                    return @"״̬֡";
                case CallId:// 3;
                    return @"���÷���ȫ�ֱ�ʶ";
                case StationType:// 3;
                    return @"���÷���վ������";
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
                case TextContent:// (byte)'T';
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