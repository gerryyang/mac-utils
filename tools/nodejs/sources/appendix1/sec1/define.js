var scope = 'global';

var f = function() {
  console.log(scope); // 輸出 undefined
  var scope = 'f';
}
f();
