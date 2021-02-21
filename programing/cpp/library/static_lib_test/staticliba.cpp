// staticliba.cpp
#include "staticliba.h"

static char g_static_szBuf[1024];

// multiple definition of `g_szBuf', so do not use global variable
//char g_szBuf[1024];

int API_A::seta()
{
	//snprintf(g_szBuf, sizeof(g_szBuf), "from seta");
	//printf("seta: g_szBuf[%s]\n", g_szBuf);

	snprintf(g_static_szBuf, sizeof(g_static_szBuf), "from seta");
	printf("seta: g_static_szBuf[%s]\n", g_static_szBuf);

	return 0;
}

int API_A::reada()
{
	//printf("reada: g_szBuf[%s]\n", g_szBuf);
	printf("reada: g_static_szBuf[%s]\n", g_static_szBuf);

	return 0;
}

int API_A::samefunc_nonstatic()
{
	printf("samefunc_nonstatic: from staticliba.cpp\n");
	return 0;
}

static int samefunc_static()
{
	printf("samefunc_static: from staticliba.cpp\n");
	return 0;
}
