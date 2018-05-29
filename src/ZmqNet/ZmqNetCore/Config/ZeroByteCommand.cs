namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public static class ZeroByteCommand
    {
        /// <summary>
        /// ��׼����
        /// </summary>
        public const byte General = (byte)0;

        /// <summary>
        /// �ȴ����
        /// </summary>
        public const byte GetGlobalId = (byte)'>';

        /// <summary>
        /// �ȴ����
        /// </summary>
        public const byte Waiting = (byte)'#';

        /// <summary>
        /// ���ҽ��
        /// </summary>
        public const byte Find = (byte)'%';

        /// <summary>
        /// �رս��
        /// </summary>
        public const byte Close = (byte)'-';

        /// <summary>
        /// Ping
        /// </summary>
        public const byte Ping = (byte)'*';

        /// <summary>
        /// ��������
        /// </summary>
        public const byte HeartJoin = (byte)'@';

        /// <summary>
        /// ��������
        /// </summary>
        public const byte HeartPitpat = (byte)'$';

        /// <summary>
        /// �����˳�
        /// </summary>
        public const byte HeartLeft = (byte)'!';
    }
}