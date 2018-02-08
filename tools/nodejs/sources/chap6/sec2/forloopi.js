//forloopi.js

var fs = require('fs');
var files = ['a.txt', 'b.txt', 'c.txt'];

for (var i = 0; i < files.length; i++) {
  fs.readFile(files[i], 'utf-8', function(err, contents) {
    console.log(files);
    console.log(i);
    console.log(files[i]);
  });
}
