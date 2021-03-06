#pragma once
#ifndef _MYDECIMAL_H
#define _MYDECIMAL_H

#include "stdinc.h"
namespace agebull
{
#define DoubleToInt64(d) static_cast<int64>(static_cast<int>(d) * 1000000LL + static_cast<int64>((d + 0.0000001 - static_cast<int>(d)) * 1000000LL))


#define Int64ToDouble(i) i / 1000000.0

#define MY_DECIMAL_ONE 1000000LL

#define MY_DECIMAL_BASE 10000LL

#define MY_DECIMAL_MIN 1000LL

	class my_decimal
	{
		double m_value;
	public:
		my_decimal(int64 value)
			: m_value(value / 1000000.0)
		{

		}
		my_decimal(double value)
			: m_value(value)
		{

		}
		my_decimal& operator *(short num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *(ushort num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *(int num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *(uint num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *(int64 num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *(uint64 num)
		{
			m_value *= num;
			return *this;
		}

		my_decimal& operator *(double num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *=(ushort num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *=(int num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *=(uint num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *=(int64 num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator *=(uint64 num)
		{
			m_value *= num;
			return *this;
		}

		my_decimal& operator *=(double num)
		{
			m_value *= num;
			return *this;
		}

		my_decimal& operator /(short num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(ushort num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(int num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(uint num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(int64 num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(uint64 num)
		{
			m_value /= num;
			return *this;
		}
		my_decimal& operator /(double num)
		{
			m_value /= num;
			return *this;
		}

		my_decimal& operator +(short num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(ushort num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(int num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(uint num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(int64 num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(uint64 num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +(double num)
		{
			m_value += num;
			return *this;
		}

		my_decimal& operator +=(short num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(ushort num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(int num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(uint num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(int64 num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(uint64 num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator +=(double num)
		{
			m_value += num;
			return *this;
		}
		my_decimal& operator -(short num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator -(ushort num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator -(int num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator -(uint num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator -(int64 num)
		{
			m_value *= num;
			return *this;
		}
		my_decimal& operator -(uint64 num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator -(double num)
		{
			m_value -= num;
			return *this;
		}
		my_decimal& operator = (double value)
		{
			m_value = value;
			return *this;
		}
		my_decimal& operator = (int64 value)
		{
			m_value = value / 1000000.0;
			return *this;
		}
		int64 round() const
		{
			int64 vl = static_cast<int64>(static_cast<int>(m_value) * 1000000LL + static_cast<int64>((m_value + 0.0000001 - static_cast<int>(m_value)) * 1000000LL));
			int64 end = vl % 1000000;
			if (end < 500000)
				return vl - end;
			return vl - end + 1000000;
		}
		operator int64() const
		{
			return static_cast<int64>(static_cast<int>(m_value) * 1000000LL + static_cast<int64>((m_value + 0.0000001 - static_cast<int>(m_value)) * 1000000LL));
		}
		double value() const
		{
			return m_value;
		}
		int64 value64() const
		{
			return static_cast<int64>(static_cast<int>(m_value) * 1000000LL + static_cast<int64>((m_value + 0.0000001 - static_cast<int>(m_value)) * 1000000LL));
		}
	};
}

#endif // !_MYDECIMAL_H