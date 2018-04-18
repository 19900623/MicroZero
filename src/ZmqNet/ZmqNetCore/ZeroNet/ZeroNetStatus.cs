using Agebull.ZeroNet.ZeroApi;
using Newtonsoft.Json;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// Zmq������
    /// </summary>
    public static class ZeroNetStatus
    {
        /// <summary>
        /// ����״̬
        /// </summary>
        public const byte ZeroStatusSuccess = (byte) '+';

        /// <summary>
        /// ����״̬
        /// </summary>
        public const byte ZeroStatusBad = (byte) '-';

        /// <summary>
        /// �ɹ�
        /// </summary>
        public const string ZeroCommandOk = "+ok";

        /// <summary>
        /// �ƻ�ִ��
        /// </summary>
        public const string ZeroCommandPlan = "+plan";

        /// <summary>
        /// ����
        /// </summary>
        public const string ZeroCommandError = "-error";

        /// <summary>
        /// ����ִ��
        /// </summary>
        public const string ZeroCommandRuning = "+runing";

        /// <summary>
        /// �ɹ��˳�
        /// </summary>
        public const string ZeroCommandBye = "+bye";

        /// <summary>
        /// �ɹ�����
        /// </summary>
        public const string ZeroCommandWecome = "+wecome";

        /// <summary>
        /// ͶƱ�ѷ���
        /// </summary>
        public const string ZeroVoteSended = "+send";

        /// <summary>
        /// ͶƱ�ѹر�
        /// </summary>
        public const string ZeroVoteClosed = "+close";

        /// <summary>
        /// ���˳�ͶƱ
        /// </summary>
        public const string ZeroVoteBye = "+bye";

        /// <summary>
        /// ͶƱ���ڽ�����
        /// </summary>
        public const string ZeroVoteWaiting = "+waiting";

        /// <summary>
        /// ͶƱ�ѿ�ʼ
        /// </summary>
        public const string ZeroVoteStart = "+start";

        /// <summary>
        /// ͶƱ�����
        /// </summary>
        public const string ZeroVoteEnd = "+end";

        /// <summary>
        /// �Ҳ�������
        /// </summary>
        public const string ZeroCommandNoFind = "-no find";

        /// <summary>
        /// ֡����
        /// </summary>
        public const string ZeroCommandInvalid = "-invalid";

        /// <summary>
        /// ��֧�ֵĲ���
        /// </summary>
        public const string ZeroCommandNoSupport = "-no support";

        /// <summary>
        /// ִ��ʧ��
        /// </summary>
        public const string ZeroCommandFailed = "-failes";

        /// <summary>
        /// ��������
        /// </summary>
        public const string ZeroCommandArgError = "-ArgumentError! must like : call[name][command][argument]";

        /// <summary>
        /// ��װʱ��������
        /// </summary>
        public const string ZeroCommandInstallArgError = "-ArgumentError! must like :install [type] [name]";

        /// <summary>
        /// ִ�г�ʱ
        /// </summary>
        public const string ZeroCommandTimeout = "-time out";

        /// <summary>
        /// ���������쳣
        /// </summary>
        public const string ZeroCommandNetError = "-net error";

        /// <summary>
        /// �Ҳ���ʵ�ʴ�����
        /// </summary>
        public const string ZeroCommandNotWorker = "-not work";

        /// <summary>
        /// δ֪����
        /// </summary>
        public const string ZeroUnknowError = "-error";

        /// <summary>
        /// ���������Json�ı�
        /// </summary>
        /// <remarks>����У�鲻ͨ��</remarks>
        public static readonly string SucceesJson = JsonConvert.SerializeObject(ApiResult.Succees());


        /// <summary>
        /// ���������Json�ı�
        /// </summary>
        /// <remarks>����У�鲻ͨ��</remarks>
        public static readonly string ArgumentErrorJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.ArgumentError));

        /// <summary>
        /// �ܾ����ʵ�Json�ı�
        /// </summary>
        /// <remarks>Ȩ��У�鲻ͨ��</remarks>
        public static readonly string DenyAccessJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.DenyAccess));

        /// <summary>
        /// δ֪�����Json�ı�
        /// </summary>
        public static readonly string UnknowErrorJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.UnknowError));

        /// <summary>
        /// ��������Json�ı�
        /// </summary>
        /// <remarks>��������Apiʱʱ�׳�δ�����쳣</remarks>
        public static readonly string NetworkErrorJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.NetworkError));

        /// <summary>
        /// �ڲ������Json�ı�
        /// </summary>
        /// <remarks>ִ�з���ʱ�׳�δ�����쳣</remarks>
        public static readonly string InnerErrorJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.InnerError));

        /// <summary>
        /// �Ҳ���������Json�ı�
        /// </summary>
        /// <remarks>����δע��</remarks>
        public static readonly string NoFindJson = JsonConvert.SerializeObject(ApiResult.Error(ZeroApi.ErrorCode.NoFind));

    }
}