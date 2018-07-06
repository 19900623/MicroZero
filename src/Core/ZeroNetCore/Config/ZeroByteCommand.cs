namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public enum ZeroByteCommand : byte
    {
        /// <summary>
        /// ��׼����
        /// </summary>
        General = (byte)1,

        /// <summary>
        /// �ƻ�����
        /// </summary>
        Plan = (byte)2,

        /// <summary>
        /// ����ִ��
        /// </summary>
        Proxy = (byte)3,

        /// <summary>
        /// �ȴ����
        /// </summary>
        GetGlobalId = (byte)'>',

        /// <summary>
        /// �ȴ����
        /// </summary>
        Waiting = (byte)'#',

        /// <summary>
        /// ���ҽ��
        /// </summary>
        Find = (byte)'%',

        /// <summary>
        /// �رս��
        /// </summary>
        Close = (byte)'-',

        /// <summary>
        /// Ping
        /// </summary>
        Ping = (byte)'*',

        /// <summary>
        /// ��������
        /// </summary>
        HeartJoin = (byte)'J',

        /// <summary>
        /// ��������
        /// </summary>
        HeartReady = (byte)'R',

        /// <summary>
        /// ��������
        /// </summary>
        HeartPitpat = (byte)'P',

        /// <summary>
        /// �����˳�
        /// </summary>
        HeartLeft = (byte)'L',
    }
}