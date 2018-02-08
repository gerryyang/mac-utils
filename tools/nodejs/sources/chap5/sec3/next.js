app.all('/user/:username', function(req, res, next) {
  console.log('all methods captured');
  next();
});
app.get('/user/:username', function(req, res) {
  res.send('user: ' + req.params.username);
});