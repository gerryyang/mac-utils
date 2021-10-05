#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <any>
#include <assert.h>

class Foo
{
public:
    Foo()
    {
        x = 100;
    }
    ~Foo() = default;

    int get()
    {
        return x;
    }

private:
    int x;
};

int main()
{
    std::cout << std::boolalpha;

    // any type
    std::any a = 1;
    std::cout << a.type().name() << ": " << std::any_cast<int>(a) << '\n';
    a = 3.14;
    std::cout << a.type().name() << ": " << std::any_cast<double>(a) << '\n';
    a = true;
    std::cout << a.type().name() << ": " << std::any_cast<bool>(a) << '\n';

    Foo foo;
    a = foo;

    assert(a.type() == typeid(Foo));
    std::cout << a.type().name() << ": " << std::any_cast<Foo>(a).get() << '\n';

    // If operand is not a null pointer, and the typeid of the requested T matches that of the contents of operand, a pointer to the value contained by operand, otherwise a null pointer
    if (auto ptr = std::any_cast<Foo>(&a))
    {
        std::cout << "perform a combined type query, true\n";
    }
    else
    {
        std::cout << "perform a combined type query, false\n";
    }

    // bad cast
    try
    {
        a = 1;
        std::cout << std::any_cast<float>(a) << '\n';
    }
    catch (const std::bad_any_cast& e)
    {
        std::cout << e.what() << '\n';
    }

    // has value
    a = 1;
    if (a.has_value())
    {
        std::cout << a.type().name() << '\n';
    }

    // reset
    a.reset();
    if (!a.has_value())
    {
        std::cout << "no value\n";
    }

    // pointer to contained data
    a = 1;
    int* i = std::any_cast<int>(&a);
    std::cout << *i << "\n";

    return 0;
}
