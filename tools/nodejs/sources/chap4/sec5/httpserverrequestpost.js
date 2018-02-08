//httpserverrequestpost.js

var http = require('http');
var querystring = require('querystring');
var util = require('util');

http.createServer(function(req, res) {
  var post = '';

  req.on('data', function(chunk) {
    post += chunk;
  });

  req.on('end', function() {
    post = querystring.parse(post);
    res.end(util.inspect(post));
  });

}).listen(3000);
