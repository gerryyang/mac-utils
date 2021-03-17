
# Usage

```
$ ./demo
  Base::Base()
  Derived::Derived()
Created a shared Derived (as a pointer to Base)
  p.get() = 0x5607c27ace80, p.use_count() = 1
Copy a shared Derived (as a pointer to Base)
  p.get() = 0x5607c27ace80, p.use_count() = 2
Release ownership from main
  p.get() = 0, p.use_count() = 0  p2.get() = 0x5607c27ace80, p2.use_count() = 1
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 2
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 4
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 4
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 5
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 7
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 7
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 8
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 9
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 10
local pointer in a thread:
  lp.get() = 0x5607c27ace80, lp.use_count() = 11
Shared ownership between threads and released
ownership from main:
  p2.get() = 0x5607c27ace80, p2.use_count() = 1
All threads completed, the last one deleted Derived
  Derived::~Derived()
  Base::~Base()
```

# Refer

* https://en.cppreference.com/w/cpp/memory/shared_ptr
