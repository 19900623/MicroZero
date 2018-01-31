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
        /// Զ�̴���
        /// </summary>
        RemoteError,
        /// <summary>
        /// �Ƿ���ʽ
        /// </summary>
        FormalError,
        /// <summary>
        /// ת��
        /// </summary>
        HttpRedirect,
        /// <summary>
        /// Http��OPTIONЭ��
        /// </summary>
        HttpOptions,
        /// <summary>
        /// �Ƿ�����
        /// </summary>
        DenyAccess
    }
}