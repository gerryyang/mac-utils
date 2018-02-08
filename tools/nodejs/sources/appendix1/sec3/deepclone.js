Object.prototype.clone = function() {
  var newObj = {};
  for (var i in this) {
    if (typeof(this[i]) == 'object' || typeof(this[i]) == 'function') {
      newObj[i] = this[i].clone();
    } else {
      newObj[i] = this[i];
    }
  }
  return newObj;
};

Array.prototype.clone = function() {
  var newArray = [];
  for (var i = 0; i < this.length; i++) {
    if (typeof(this[i]) == 'object' || typeof(this[i]) == 'function') {
      newArray[i] = this[i].clone();
    } else {
      newArray[i] = this[i];
    }
  }
  return newArray;
};

Function.prototype.clone = function() {
  var that = this;
  var newFunc = function() {
    return that.apply(this, arguments); 
  };
  for (var i in this) {
    newFunc[i] = this[i];
  }
  return newFunc;
};

var obj = {
  name: 'byvoid',
  likes: ['node'],
  display: function() {
    console.log(this.name);
  },
};

var newObj = obj.clone();
newObj.likes.push('python');
console.log(obj.likes); // 輸出 [ 'node' ]
console.log(newObj.likes); // 輸出 [ 'node', 'python' ]
console.log(newObj.display == obj.display); // 輸出 false
