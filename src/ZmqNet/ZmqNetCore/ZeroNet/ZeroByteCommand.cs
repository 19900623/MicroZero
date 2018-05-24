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
        public const byte Waiting = (byte)'#';

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