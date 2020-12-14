#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <limits>
#include <cmath>
#include "singleton.h"
#include "../include/error_code.h"

class Calculator : public Singleton<Calculator>
{
	public:
		template<typename ValueType>
		ValueType ValueTypeMax(ValueType)
		{
			return std::numeric_limits<ValueType>::max();
		}

		int Exp2(double &exp2, const double &val);
};

#endif
