using System;
using System.Reflection;
using Agebull.Common.Logging;
using Newtonsoft.Json;
using Agebull.ZeroNet.ZeroApi;

namespace Agebull.ZeroNet.ZeroApi
{
    /// <summary>
    /// Apiվ��
    /// </summary>
    public abstract class ApiAction
    {
        /// <summary>
        /// Api����
        /// </summary>
        public string Name { get; set; }
        /// <summary>
        /// ��Ҫ��¼
        /// </summary>
        public bool NeedLogin { get; set; }
        /// <summary>
        /// �Ƿ񹫿��ӿ�
        /// </summary>
        public bool IsPublic { get; set; }
        /// <summary>
        /// ��ԭ����
        /// </summary>
        public abstract bool RestoreArgument(string argument);

        /// <summary>
        /// ����У��
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public abstract bool Validate(out string message);

        /// <summary>
        /// ִ��
        /// </summary>
        /// <returns></returns>
        public abstract IApiResult Execute();
    }
    /// <summary>
    /// Api����
    /// </summary>
    public sealed class ApiAction<TResult> : ApiAction
        where TResult : IApiResult
    {
        /// <summary>
        /// ��ԭ����
        /// </summary>
        public override bool RestoreArgument(string argument)
        {
            return true;
        }

        /// <summary>
        /// ����У��
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public override bool Validate(out string message)
        {
            message = null;
            return true;
        }

        /// <summary>
        /// ִ����Ϊ
        /// </summary>
        public Func<TResult> Action { get; set; }
        /// <summary>
        /// ִ��
        /// </summary>
        /// <returns></returns>
        public override IApiResult Execute()
        {
            return Action();
        }
    }
    /// <summary>
    /// API��׼ί��
    /// </summary>
    /// <param name="argument"></param>
    /// <returns></returns>
    public delegate IApiResult ApiDelegate(IApiArgument argument);

    /// <summary>
    /// API��׼ί��
    /// </summary>
    /// <returns></returns>
    public delegate IApiResult ApiDelegate2();

    /// <summary>
    /// Api����
    /// </summary>
    public sealed class AnyApiAction<TControler> : ApiAction
        where TControler : class, new()
    {
        /// <summary>
        /// ��������
        /// </summary>
        public Type ArgumenType { get; set; }

        private IApiArgument _argument;
        /// <summary>
        /// ��ԭ����
        /// </summary>
        public override bool RestoreArgument(string argument)
        {
            if (ArgumenType == null)
                return true;
            try
            {
                _argument = JsonConvert.DeserializeObject(argument, ArgumenType) as IApiArgument;
                return _argument != null;
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return false;
            }
        }

        /// <summary>
        /// ����У��
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public override bool Validate(out string message)
        {
            if (_argument == null)
            {
                message = "��������Ϊ��";
                return false;
            }
            return _argument.Validate(out message);
        }
        /// <summary>
        /// ִ����Ϊ
        /// </summary>
        public MethodInfo Method { get; set; }

        /// <summary>
        /// ִ��
        /// </summary>
        /// <returns></returns>
        public override IApiResult Execute()
        {
            if (ArgumenType == null)
            {
                var action = Method.CreateDelegate(typeof(ApiDelegate2),new TControler());
                return action.DynamicInvoke() as IApiResult;
            }
            else
            {
                var action = Method.CreateDelegate(typeof(ApiDelegate),new TControler());
                return action.DynamicInvoke(_argument) as IApiResult;
            }
        }
    }

    /// <summary>
    /// Api����
    /// </summary>
    public sealed class ApiAction<TArgument, TResult> : ApiAction
        where TArgument : class, IApiArgument
        where TResult : IApiResult
    {
        private TArgument _argument;
        /// <summary>
        /// ��ԭ����
        /// </summary>
        public override bool RestoreArgument(string argument)
        {
            try
            {
                _argument = JsonConvert.DeserializeObject<TArgument>(argument);
                return _argument != null;
            }
            catch (Exception e)
            {
                LogRecorder.Exception(e);
                return false;
            }
        }

        /// <summary>
        /// ����У��
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        public override bool Validate(out string message)
        {
            return _argument.Validate(out message);
        }

        /// <summary>
        /// ִ����Ϊ
        /// </summary>
        public Func<TArgument, TResult> Action { get; set; }
        /// <summary>
        /// ִ��
        /// </summary>
        /// <returns></returns>
        public override IApiResult Execute()
        {
            return Action(_argument);
        }
    }
}