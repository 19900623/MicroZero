#ifndef _AGEBULL_TSON_H
#define _AGEBULL_TSON_H
#pragma once
//Type Serialize Object Notation
namespace Agebull
{
	namespace Tson
	{
		//���л�ͷ��С������4+����4+�汾1+����1��
		const unsigned int SERIALIZE_HEAD_LEN = 10;
		//���л�������С������4+����4+�汾1+����1+����4��
		const unsigned int SERIALIZE_BASE_LEN = 14;

		typedef unsigned int OBJ_TYPEID;
		typedef unsigned char OBJ_VERSION;
		typedef unsigned char OBJ_TYPE;
		typedef unsigned char FIELD_INDEX;
		typedef unsigned int TYPE_INDEX;

		const OBJ_TYPE OBJ_TYPE_BOF = 0x0;
		const OBJ_TYPE OBJ_TYPE_EOF = 0xFF;
		const OBJ_TYPE OBJ_TYPE_END = 0xFF;
		/*δʵ�� �������ͱ�ʶ
		BUFFͷΪ( ����(4),�������ͱ�ʶ(2),���ݰ汾��ʶ(2))

		��ÿһ���ֶε� �ṹΪ(�ֶα�ʶ(1),�ֶ�����(1),�ֶγ���(�ı�\������\�������),�ֶ�����)

		����������������0(�ֶ�����Ϊ��)

		�����ĺô���,�ṹ���岻ͬ(��Ϊ����ͬ),Ҳ������ȷ�Ļ�ԭ

		����汾����,��������Ƚϴ�ʱ,����ͨ����ͬ�汾�Ķ�ȡ��ʵ�ּ���

		���½ṹ��Ϊ����˵��,������

		typedef struct
		{
		FIELD_INDEX field_index;
		OBJ_TYPE field_type;
		char buffer[1];
		}ASON_FIELD;

		typedef struct
		{
		FIELD_INDEX field_index;
		OBJ_TYPE field_type;
		int len;
		char buffer[1];
		}ASON_FIELD_ARRAY;

		typedef struct
		{
		int len;
		OBJ_TYPEID type_id;
		OBJ_VERSION type_version;
		ASON_FIELD property[1];
		}ASONBUFFER;

		*/

		/*
		const OBJ_TYPE OBJ_TYPE_BOOLEN = 0x1;
		const OBJ_TYPE OBJ_TYPE_CHAR = 0x2;
		const OBJ_TYPE OBJ_TYPE_BYTE = 0x3;
		const OBJ_TYPE OBJ_TYPE_INT16 = 0x4;
		const OBJ_TYPE OBJ_TYPE_UINT16 = 0x5;
		const OBJ_TYPE OBJ_TYPE_INT32 = 0x6;
		const OBJ_TYPE OBJ_TYPE_UINT32 = 0x7;
		const OBJ_TYPE OBJ_TYPE_INT64 = 0x8;
		const OBJ_TYPE OBJ_TYPE_UINT64 = 0x9;
		const OBJ_TYPE OBJ_TYPE_FLOAT = 0xA;
		const OBJ_TYPE OBJ_TYPE_DOUBLE = 0xB;
		const OBJ_TYPE OBJ_TYPE_DATETIME = 0xC;

		const OBJ_TYPE OBJ_TYPE_BOOLEN_ARRAY = 0x11;
		const OBJ_TYPE OBJ_TYPE_CHAR_ARRAY = 0x12;
		const OBJ_TYPE OBJ_TYPE_BYTE_ARRAY = 0x13;
		const OBJ_TYPE OBJ_TYPE_INT16_ARRAY = 0x14;
		const OBJ_TYPE OBJ_TYPE_UINT16_ARRAY = 0x15;
		const OBJ_TYPE OBJ_TYPE_INT32_ARRAY = 0x16;
		const OBJ_TYPE OBJ_TYPE_UINT32_ARRAY = 0x17;
		const OBJ_TYPE OBJ_TYPE_INT64_ARRAY = 0x18;
		const OBJ_TYPE OBJ_TYPE_UINT64_ARRAY = 0x19;
		const OBJ_TYPE OBJ_TYPE_FLOAT_ARRAY = 0x1A;
		const OBJ_TYPE OBJ_TYPE_DOUBLE_ARRAY = 0x1B;
		const OBJ_TYPE OBJ_TYPE_DATETIME_ARRAY = 0x1C;

		const OBJ_TYPE OBJ_TYPE_STRING = 128;
		const OBJ_TYPE OBJ_TYPE_BINARY = 129;
		const OBJ_TYPE OBJ_TYPE_OBJECT = 130;
		const OBJ_TYPE OBJ_TYPE_OBJECT_ARRAY = 131;
		*/

	}
}

#endif

