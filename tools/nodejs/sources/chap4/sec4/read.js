var fs = require('fs');

fs.open('content.txt', 'r', function(err, fd) {
  if (err) {
    console.error(err);
    return;
  }
  
  var buf = new Buffer(8);
  fs.read(fd, buf, 0, 8, null, function(err, bytesRead, buffer) {
    if (err) {
      console.error(err);
      return;
    }
    
    console.log('bytesRead: ' + bytesRead);
    console.log(buffer);
  })
});
