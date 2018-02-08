//將func = someuser.func.bind(foo)展開：
func = function() {
  return someuser.func.call(foo);
};

//再將func2 = func.bind(someuser)展開：
func2 = function() {
  return func.call(someuser);
};
