
# Usage

```
$ ./demo 
  Base::Base()
  Derived::Derived()
Created a shared Derived (as a pointer to Base)
  p.get() = 0x5580acf9fe80, p.use_count() = 1
release ownership from main
Shared ownership between 3 threads and released
ownership from main:
  p.get() = 0, p.use_count() = 0
local pointer in a thread:
  lp.get() = 0x5580acf9fe80, lp.use_count() = 4
local pointer in a thread:
  lp.get() = 0x5580acf9fe80, lp.use_count() = 3
local pointer in a thread:
  lp.get() = 0x5580acf9fe80, lp.use_count() = 2
  Derived::~Derived()
  Base::~Base()
All threads completed, the last one deleted Derived
```

