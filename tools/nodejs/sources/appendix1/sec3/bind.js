var someuser = {
  name: 'byvoid',
  func: function() {
    console.log(this.name);
  }
};

var foo = {
  name: 'foobar'
};

foo.func = someuser.func;
foo.func(); // 輸出 foobar

foo.func1 = someuser.func.bind(someuser);
foo.func1(); // 輸出 byvoid

func = someuser.func.bind(foo);
func(); // 輸出 foobar

func2 = func;
func2(); // 輸出 foobar
