app.get('/user/:username', function(req, res) {
  res.send('user: ' + req.params.username);
});
