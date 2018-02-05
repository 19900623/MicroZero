namespace ZeroNet.Http.Route
{
    /// <summary>
    /// һ��·��ִ��״̬
    /// </summary>
    public enum RouteStatus
    {
        /// <summary>
        /// ����
        /// </summary>
        None,
        /// <summary>
        /// ����
        /// </summary>
        Cache,
        /// <summary>
        /// Http��OPTIONЭ��
        /// </summary>
        HttpOptions,
        /// <summary>
        /// �Ƿ���ʽ
        /// </summary>
        FormalError,
        /// <summary>
        /// ���ش���
        /// </summary>
        LocalError,
        /// <summary>
        /// Զ�̴���
        /// </summary>
        RemoteError,
        /// <summary>
        /// �Ƿ�����
        /// </summary>
        DenyAccess
    }
}