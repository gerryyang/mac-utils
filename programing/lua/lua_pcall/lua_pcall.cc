#include<iostream>
#include<string>

extern "C" {
#include<lua.h>
#include<lualib.h>
#include<lauxlib.h>
}

int main()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	if (luaL_loadfile(L, "test.lua"))
	{
		std::cout << "open file error" << std::endl;
		return 1;
	}

	// run test.lua
	int ret = lua_pcall(L, 0, 0, 0);
	if (ret)
	{
		std::cout << "1 function call error: " << ret << std::endl;
	}

	// function to be called
	lua_getglobal(L, "errorhandle");
	lua_getglobal(L, "printmsg");

	// run test.lua and call printmsg but no errfunc
	ret = lua_pcall(L, 0, 0, 0);
	if (ret)
	{
		std::cout << "2 function call error: " << ret << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
	}

	// run test.lua and call printmsg and set errfunc
	ret = lua_pcall(L, 0, 0, -2);
	if (ret)
	{
		std::cout << "3 function call error: " << ret << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
	}

	lua_close(L);

	return 0;
}

/*
$ ./a.out 
2 function call error: 2
test.lua:7: attempt to call a nil value (global 'print_not_exist')
3 function call error: 2
ATTEMPT TO CALL A STRING VALUE
*/
