using System;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using Agebull.Common.Mvvm;
using Agebull.MicroZero;
using Agebull.MicroZero.ApiDocuments;
using Agebull.MicroZero.ZeroManagemant;

namespace Agebull.EntityModel.Designer
{
    /// <summary>
    /// ���γ���ģ��
    /// </summary>
    public class TreeModel : DesignModelBase
    {
        #region ����

        /// <summary>
        ///     ��ǰ������
        /// </summary>
        public TreeRoot TreeRoot { get; }

        /// <summary>
        /// 
        /// </summary>
        public TreeModel()
        {
            TreeRoot = new TreeRoot();
        }
        /// <summary>
        /// ��ʼ��
        /// </summary>
        protected override void DoInitialize()
        {
        }
        /// <summary>
        /// ͬ������������
        /// </summary>
        public override void OnSolutionChanged()
        {
            using (WorkModelScope.CreateScope(WorkModel.Show))
                CreateTree();
        }
        /// <summary>
        /// 
        /// </summary>
        public void CreateTree()
        {
            TreeRoot.SelectItemChanged -= OnTreeSelectItemChanged;
            TreeRoot.Items.Clear();
            Task.Factory.StartNew(CreateTreeTask);
        }

        #endregion


        #region ����

        void CreateTreeTask()
        {
            ZeroApplication.Run();
            var icon = Application.Current.Resources["tree_Folder"] as BitmapImage;
            foreach (var station in ZeroApplication.Config.GetConfigs())
            {
                var node = new TreeItem<StationConfig>(station)
                {
                    IsExpanded = false,
                    Header = station.Caption ?? station.Name,
                    SoruceTypeIcon = icon
                };
                CreateStationTreeItem(node, station);
                TreeRoot.Items.Add(node);
            }
            TreeRoot.SelectItemChanged += OnTreeSelectItemChanged;
        }

        private void CreateStationTreeItem(TreeItem<StationConfig> node, StationConfig config)
        {
            if (!SystemManager.Instance.LoadDocument(config.StationName, out var doc))
            {
                return;
            }

            var icon = Application.Current.Resources["tree_item"] as BitmapImage;
            foreach (var api in doc.Aips.Values)
            {
                var eitem = new TreeItem<ApiDocument>(api)
                {
                    Header = api.Caption ?? api.Name,
                    SoruceTypeIcon = icon
                };
                node.Items.Add(eitem);
            }
        }


        #endregion


        #region ѡ��

        private void OnTreeSelectItemChanged(object sender, EventArgs e)
        {
            SetSelect(sender as TreeItem);
        }

        /// <summary>
        ///     ��ǰѡ��
        /// </summary>
        public TreeItem SelectItem => TreeRoot.SelectItem;


        public void SetSelect(TreeItem item)
        {
            TreeRoot.SelectItem = item;
            RaisePropertyChanged(nameof(SelectItem));
        }

        #endregion

        #region ����
        /// <summary>
        /// ����
        /// </summary>
        public CommandItem FindCommand => new CommandItem
        {
            Caption = "����",
            Action = Find
        };

        /// <summary>
        /// ����
        /// </summary>
        public void Find(object arg)
        {
        }

        #endregion
    }
}