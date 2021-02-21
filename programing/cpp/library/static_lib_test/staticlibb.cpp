// staticlibb.cpp
#include "staticlibb.h"

static char g_static_szBuf[1024];

// multiple definition of `g_szBuf', so do not use global variable
//char g_szBuf[1024];

int API_B::setb()
{
	//snprintf(g_szBuf, sizeof(g_szBuf), "from setb");
	//printf("setb: g_szBuf[%s]\n", g_szBuf);

	snprintf(g_static_szBuf, sizeof(g_static_szBuf), "from setb");
	printf("setb: g_static_szBuf[%s]\n", g_static_szBuf);

	return 0;
}

int API_B::readb()
{
	//printf("readb: g_szBuf[%s]\n", g_szBuf);
	printf("readb: g_static_szBuf[%s]\n", g_static_szBuf);

	return 0;
}

int API_B::samefunc_nonstatic()
{
	printf("samefunc_nonstatic: from staticlibb.cpp\n");
	return 0;
}

static int samefunc_static()
{
	printf("samefunc_static: from staticlibb.cpp\n");
	return 0;
}

