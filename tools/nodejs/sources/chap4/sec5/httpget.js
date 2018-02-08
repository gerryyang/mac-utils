//httpget.js

var http = require('http');

http.get({host: 'www.byvoid.com'}, function(res) {
  res.setEncoding('utf8');
  res.on('data', function (data) {
    console.log(data);
  });
});
