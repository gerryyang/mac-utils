var someuser = {
  name: 'byvoid',
  func: function () {
    console.log(this.name);
  }
};

var foo = {
  name: 'foobar'
};

func = someuser.func.bind(foo);
func(); // 輸出 foobar

func2 = func.bind(someuser);
func2(); // 輸出 foobar
