var someuser = {
  name: 'byvoid',
  func: function() {
    console.log(this.name);
  }
};

var foo = {
  name: 'foobar'
};

someuser.func(); // 輸出 byvoid

foo.func = someuser.func;
foo.func(); // 輸出 foobar

name = 'global';
func = someuser.func;
func(); // 輸出 global
