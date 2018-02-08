var v1 = 'v1';

var f1 = function() {
  console.log(v1); // 輸出 v1
};
f1();

var f2 = function() {
  var v1 = 'local';
  console.log(v1); // 輸出 local
};
f2();
