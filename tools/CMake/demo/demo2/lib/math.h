// util/math.h
#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#include "error_code.h"
#include <limits>

template<typename ValueType>
ValueType ValueTypeMax(ValueType)
{
	return std::numeric_limits<ValueType>::max();
}

template<typename ValueType>
int SafeAdd(ValueType &sum)
{
	return exit_success;
}

template<typename ValueType, typename ...ValueTypes>
int SafeAdd(ValueType &sum, const ValueType &value, const ValueTypes &...other_values)
{
	int ret_val = SafeAdd<ValueType>(sum, other_values...);
	if (ret_val != exit_success)
	{
		return ret_val;
	}
	if (sum > ValueTypeMax(value) - value)
	{
		sum = ValueTypeMax(value);
		return error_data_overflow;
	}
	sum += value;
	return exit_success;
}

#endif
