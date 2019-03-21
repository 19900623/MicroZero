// // /*****************************************************
// // (c)2016-2016 Copy right www.gboxt.com
// // ����:
// // ����:Agebull.DataModel
// // ����:2016-06-12
// // �޸�:2016-06-24
// // *****************************************************/

#region ����

using System.ComponentModel;
using System.Runtime.Serialization;
using Agebull.Common.DataModel;
using Agebull.Common.Rpc;
using Agebull.ZeroNet.ZeroApi;
using Newtonsoft.Json;

#endregion

namespace Xuhui.Internetpro.WzHealthCardService
{
    /// <summary>
    ///     Ϊҵ���������Ķ���
    /// </summary>
    [DataContract]
    [JsonObject(MemberSerialization.OptIn)]
    public class WzHealthCardContext : GlobalContext
    {
        /// <summary>
        ///     ȡ�û������̵߳������󣬵�ǰ���󲻴���ʱ�����Զ�����һ��
        /// </summary>
        public static WzHealthCardContext Context => Current as WzHealthCardContext;

        /// <summary>
        /// ��ǰ����
        /// </summary>
        private ApiArgument _argument;

        /// <summary>
        /// ��ǰ����
        /// </summary>
        public ApiArgument Argument
        {
            get
            {
                if (_argument != null)
                    return _argument;
                return _argument = DependencyObjects.Dependency<ApiAction>()?.Argument as ApiArgument;
            }

        }
    }
}