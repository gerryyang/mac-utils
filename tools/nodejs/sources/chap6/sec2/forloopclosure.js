//forloopclosure.js

var fs = require('fs');
var files = ['a.txt', 'b.txt', 'c.txt'];

for (var i = 0; i < files.length; i++) {
  (function(i) {
    fs.readFile(files[i], 'utf-8', function(err, contents) {
      console.log(files[i] + ': ' + contents);
    });
  })(i);
}
