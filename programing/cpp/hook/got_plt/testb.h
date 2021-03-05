#pragma once

class Foo
{
public:
	void say_world();
};

#ifdef __cplusplus
extern "C" {
#endif

void say_world();

#ifdef __cplusplus
};/* extern "C" */
#endif
