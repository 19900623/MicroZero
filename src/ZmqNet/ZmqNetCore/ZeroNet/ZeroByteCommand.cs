namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public static class ZeroByteCommand
    {
        /// <summary>
        /// ����վ�����
        /// </summary>
        public const string WorkerJoin = "@";
        /// <summary>
        /// ����վ�����
        /// </summary>
        public const string WorkerLeft = "!";
        /// <summary>
        /// ����վ��ȴ�����
        /// </summary>
        public const string WorkerListen = "$";

        /// <summary>
        /// ��������
        /// </summary>
        public const string HeartJoin = "@";

        /// <summary>
        /// ��������
        /// </summary>
        public const string HeartPitpat = "$";

        /// <summary>
        /// �����˳�
        /// </summary>
        public const string HeartLeft = "!";
    }
}