

# CentOS

yum install python-redis


# Refer

[if __name__ == '__main__' 如何正确理解?](https://www.zhihu.com/question/49136398)

[unittest — Unit testing framework](https://docs.python.org/3/library/unittest.html)

``` python
import unittest

class TestStringMethods(unittest.TestCase):

    def test_upper(self):
        self.assertEqual('foo'.upper(), 'FOO')

    def test_isupper(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)

if __name__ == '__main__':
    unittest.main()
```

[浅谈 Python 的 with 语句](https://www.ibm.com/developerworks/cn/opensource/os-cn-pythonwith/index.html)

[pprint — Data pretty printer](https://docs.python.org/3/library/pprint.html)