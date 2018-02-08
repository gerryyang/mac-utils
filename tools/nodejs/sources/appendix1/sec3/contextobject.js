var someuser = {
  name: 'byvoid',
  display: function() {
    console.log(this.name);
  }
};

someuser.display(); // 輸出 byvoid

var foo = {
  bar: someuser.display,
  name: 'foobar'
};

foo.bar(); // 輸出 foobar
