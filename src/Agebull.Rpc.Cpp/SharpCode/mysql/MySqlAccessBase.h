#pragma once
#include "../mysql/DBMySQL.h"


namespace GBS
{
	namespace Futures
	{
		namespace DataModel
		{
			template<class TData, int FieldCount>
			class MySqlAccessBase
			{
			public:
				//���ݿ����
				std::shared_ptr<CDBMySQL> m_msql;
			public:
				/**
				* @brief ����
				*/
				virtual ~MySqlAccessBase()
				{
					m_msql->Close();
				}

				/**
				* @brief Ĭ�Ϲ���
				*/
				MySqlAccessBase()
					: m_msql(new CDBMySQL())
				{
					m_msql->Open();
				}
				/**
				* @brief ���ƹ���
				* @param {shared_ptr<CDBMySQL>} mysql ���ݿ����
				*/
				MySqlAccessBase(shared_ptr<CDBMySQL> mysql)
					: m_msql(mysql)
				{
				}
				/**
				* @brief ����SQL�������
				* @param {TData} data ����
				*/
				virtual string InsertSql(TData data) = 0;

				/**
				* @brief ����
				* @param {DATA_STATE} state ����״̬
				*/
				void Insert(TData data)
				{
					string sql = InsertSql(data);
					string key = m_msql->ExecuteScalar(sql);
					data.SetValue(nullptr, key.c_str());//���ֶα�ʾд����
				}
				/**
				* @brief ����SQL�������
				* @param {TData} data ����
				*/
				virtual string UpdateSql(TData data) = 0;

				/**
				* @brief ����
				* @param {DATA_STATE} state ����״̬
				*/
				void Update(TData data)
				{
					string sql = UpdateSql(data);
					m_msql->Execute(sql);
				}
				/**
				* @brief ��ѯSQL�������
				* @param {string} condition ��ѯ����
				*/
				virtual string QuerySql(string condition) = 0;

				/**
				* @brief ��ѯ
				* @param {string} condition ��ѯ����
				*/
				vector<TData> Query(string condition)
				{
					string sql = QuerySql(condition);
					return m_msql->Query<TData>(sql);
				}
				/**
				* @brief ɾ��SQL�������
				* @param {string} condition ɾ������
				*/
				virtual string DeleteSql(string condition) = 0;

				/**
				* @brief ɾ��
				* @param {string} condition ɾ������
				*/
				void Delete(string condition)
				{
					string sql = DeleteSql(condition);
					m_msql->Execute(sql);
				}
			};
		}
	}

}

