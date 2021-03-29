
# Usage

```
$ ./type_id
myint has type: i
mystr has type: NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
mydoubleptr has type: Pd
50
std::cout<<myint has type : So
printf("%d\n",myint) has type : i
reference to non-polymorphic base: 4Base
reference to polymorphic base: 8Derived2
mydoubleptr points to d
bad_ptr points to...  caught std::bad_typeid


$ ./type_index 
i is int
d is double
a is A
b is B
c is C


$ ./type
{ {}, 42, 123, 3.14159, 2.71828, "C++17", }
Unregistered type "y"
Register visitor for type "y"
0xf
```

# Refer

* https://en.cppreference.com/w/cpp/language/typeid
* https://en.cppreference.com/w/cpp/types/type_info
* https://en.cppreference.com/w/cpp/utility/any/type
* https://en.cppreference.com/w/cpp/types/type_index
