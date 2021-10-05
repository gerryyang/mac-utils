// staticlibb.h
#ifndef _API_B_H_
#define _API_B_H_

#include <cstdio>

namespace API_B
{
int setb();
int readb();

// namespace used to prevent multiple definition of `samefunc_nonstatic()'
int samefunc_nonstatic();

//static int samefunc_static();

}

#endif
