using System;
using System.Collections.Generic;

namespace MicroZero.Http.Route
{
    /// <summary>
    /// ����ʱ����ڵ�
    /// </summary>
    public class RuntimeWaringItem
    {
        /*��ֹʱ��*/
        public DateTime StartTime, LastTime;
        /// <summary>
        /// ���һ�η�����ʱ��
        /// </summary>
        public DateTime MessageTime;
        /// <summary>
        /// �������������ʹ��������ͺ�������
        /// </summary>
        public int WaringCount, SendCount, LastCount;
        /// <summary>
        /// ���������API
        /// </summary>
        public Dictionary<string, List<string>> Apis = new Dictionary<string, List<string>>();
    }
}