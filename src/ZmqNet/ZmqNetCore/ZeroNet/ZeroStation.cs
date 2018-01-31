using System.Threading;
using Gboxt.Common.DataModel;

namespace Agebull.ZeroNet.Core
{
    /// <summary>
    /// վ��
    /// </summary>
    public abstract class ZeroStation
    {
        /// <summary>
        /// վ������
        /// </summary>
        public string StationName { get; set; }
        /// <summary>
        /// ʵ������
        /// </summary>
        private string _realName;

        /// <summary>
        /// ʵ������
        /// </summary>
        public string RealName => _realName ?? (_realName = StationName + "-" + RandomOperate.Generate(5));

        /// <summary>
        /// վ������
        /// </summary>
        public StationConfig Config { get; set; }

        /// <summary>
        /// ����״̬
        /// </summary>
        public StationState RunState;

        /// <summary>
        /// �ر�
        /// </summary>
        /// <returns></returns>
        public bool Close()
        {
            if (RunState == StationState.Run)
            {
                RunState = StationState.Closing;
                Thread.Sleep(100);
            }
            RunState = StationState.Closed;
            return true;
        }

        /// <summary>
        /// ������ѯ
        /// </summary>
        /// <returns></returns>
        public abstract bool Run();

        /// <summary>
        /// ִ��
        /// </summary>
        /// <param name="station"></param>
        public static void Run(ZeroStation station)
        {
            station.Close();
            station.Config = StationProgram.GetConfig(station.StationName);
            if (station.Config == null)
            {
                StationProgram.WriteLine($"{station.StationName} not find,try install...");
                StationProgram.InstallApiStation(station.StationName);
                return;
            }
            station.Run();
        }
    }
}