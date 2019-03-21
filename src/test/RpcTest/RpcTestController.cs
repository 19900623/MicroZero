using Agebull.MicroZero.ZeroApi;
using Agebull.MicroZero.ZeroApis;

namespace RpcTest
{
    /// <summary>
    /// Weixin����
    /// </summary>
    public class RpcTestController : ApiController
    {
        /// <summary>
        /// ������������
        /// </summary>
        /// <returns></returns>
        [Route("v1/msg/text")]
        public ApiResult OnTextRequest()
        {

            return ApiResult.Ok;
        }
    }
}