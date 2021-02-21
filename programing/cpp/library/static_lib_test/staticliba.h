// staticliba.h
#ifndef _API_A_H_
#define _API_A_H_

#include <cstdio>

namespace API_A
{

	int seta();
	int reada();

	// namespace used to prevent multiple definition of `samefunc_nonstatic()'
	int samefunc_nonstatic();

	//static int samefunc_static();

}

#endif
