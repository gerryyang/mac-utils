var f = function() {
  var scope = 'f0';
  (function() {
    var scope = 'f1';
    (function() {
      console.log(scope); // 輸出 f1
    })();
  })();
};
f();
