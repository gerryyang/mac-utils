var users = {
  'byvoid': {
    name: 'Carbo',
    website: 'http://www.byvoid.com'
  }
};

app.all('/user/:username', function(req, res, next) {
  // 檢查用戶是否存在
  if (users[req.params.username]) {
    next();
  } else {
    next(new Error(req.params.username + ' does not exist.'));
  }
});
app.get('/user/:username', function(req, res) {
  // 用戶一定存在，直接展示
  res.send(JSON.stringify(users[req.params.username]));
});
app.put('/user/:username', function(req, res) {
  // 修改用戶信息
  res.send('Done');
});
