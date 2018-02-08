//callbackforeach.js

var fs = require('fs');
var files = ['a.txt', 'b.txt', 'c.txt'];

files.forEach(function(filename) {
  fs.readFile(filename, 'utf-8', function(err, contents) {
    console.log(filename + ': ' + contents);
  });
});
