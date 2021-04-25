

# Performance 

* `std::map`查找速度是`pb repeated`的10倍
* `std::vector`是`std::map`的5倍
* `std::unordered_map`是`std::vector`的33倍

| `pb repeated` | `std::vector` | `std::map` | `std::unordered_map`
| -- | -- | -- | --
| 0.226s | 0.00468892s |  0.0250801s | 0.000139513s


