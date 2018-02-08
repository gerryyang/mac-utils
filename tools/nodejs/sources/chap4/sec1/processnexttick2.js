function doSomething(args, callback) {
  somethingComplicated(args);
  process.nextTick(callback);
}

doSomething(function onEnd() {
  compute();
});