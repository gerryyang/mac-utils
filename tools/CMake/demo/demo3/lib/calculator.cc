#include "calculator.h"

int Calculator::Exp2(double &exp2, const double &val)
{
	if (std::sqrt(ValueTypeMax(val)) < val)
	{
		exp2 = ValueTypeMax(val);
		return error_data_overflow;
	}
	exp2 = std::exp2(val);
	return exit_success;
}
