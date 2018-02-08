var generateClosure = function() {
  var count = 0;
  var get = function() {
    count ++;
    return count;
  };
  return get;
};

var counter1 = generateClosure();
var counter2 = generateClosure();
console.log(counter1()); // 輸出 1
console.log(counter2()); // 輸出 1
console.log(counter1()); // 輸出 2
console.log(counter1()); // 輸出 3
console.log(counter2()); // 輸出 2
