var fs = require('fs');

fs.readFile('content.txt', function(err, data) {
  if (err) {
    console.error(err);
  } else {
    console.log(data);
  }
});
