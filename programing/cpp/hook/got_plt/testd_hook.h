#pragma once
#include "testd.h"

namespace HotPatch
{

class TestD : public Base
{
public:
    TestD() : m_a(2) {}
    void say_hello(int a) override;
    static void say_hello_hotpatch(TestD* obj, int a);

    void call_hotpatch_function_from_another_file(int a);

private:
    void say_hello_hotpatch_impl(int a);

private:
    int m_a;
};

}