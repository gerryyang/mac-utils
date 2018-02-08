//readfilecallback.js

function readFileCallBack(err, data) {
  if (err) {
    console.error(err);
  } else {
    console.log(data);
  }
}

var fs = require('fs');
fs.readFile('file.txt', 'utf-8', readFileCallBack);
console.log('end.');
