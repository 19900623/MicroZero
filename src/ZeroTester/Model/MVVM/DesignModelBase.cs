using Agebull.Common.Mvvm;

namespace Agebull.EntityModel.Designer
{
    public class DesignModelBase : ModelBase
    {
        #region ��������
        
        /// <summary>
        ///     ����
        /// </summary>
        public string EditorName { get; set; }

        NotificationList<CommandItemBase> _commands;
        public NotificationList<CommandItemBase> Commands => _commands ??  (_commands=CreateCommands());

        /// <summary>
        /// �����������
        /// </summary>
        /// <returns></returns>
        public virtual NotificationList<CommandItemBase> CreateCommands()
        {
            var commands = new NotificationList<CommandItemBase>();
            CreateCommands(commands);

            return commands;
        }

        /// <summary>
        /// �����������
        /// </summary>
        /// <param name="commands"></param>
        protected virtual void CreateCommands(NotificationList<CommandItemBase> commands)
        {
        }

        #endregion

        #region ��������

        /// <summary>
        /// ͬ������������
        /// </summary>
        public virtual void OnSolutionChanged()
        {
        }

        /// <summary>
        /// ������
        /// </summary>
        private DataModelDesignModel _model;

        /// <summary>
        /// ����ģ��
        /// </summary>
        public DataModelDesignModel Model
        {
            get => _model;
            set
            {
                _model = value;
                RaisePropertyChanged(nameof(Model));
            }
        }

        #endregion

    }
}