

[My template specialization differs debug version from release version, is this gcc bug?](https://stackoverflow.com/questions/39976307/my-template-specialization-differs-debug-version-from-release-version-is-this-g)

这个问题和强弱符号覆盖有关，按照C++标准建议是在使用特化版本的时候显式包含，否则存在未定义行为。



