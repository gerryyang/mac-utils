#pragma once

class Base
{
public:
    virtual ~Base() {}
    virtual void say_hello(int a) = 0;
};

class TestD : public Base
{
public:
    TestD() : m_a(1) {}
    void say_hello(int a) override;

public:
    void say_hello_impl(int a);  // replaced function

public:
    int m_a;
};
