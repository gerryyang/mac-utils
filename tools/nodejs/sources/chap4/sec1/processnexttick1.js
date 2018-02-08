function doSomething(args, callback) {
  somethingComplicated(args);
  callback();
}

doSomething(function onEnd() {
  compute();
});