Object.prototype.clone = function() {
  var newObj = {};
  for (var i in this) {
    newObj[i] = this[i];
  }
  return newObj;
}

var obj = {
  name: 'byvoid',
  likes: ['node']
};

var newObj = obj.clone();
obj.likes.push('python');

console.log(obj.likes); // 輸出 [ 'node', 'python' ]
console.log(newObj.likes); // 輸出 [ 'node', 'python' ]
