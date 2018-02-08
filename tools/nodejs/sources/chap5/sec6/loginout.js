app.get('/login', function(req, res) {
  res.render('login', {
    title: '用戶登入',
  });
});

app.post('/login', function(req, res) {
  //生成口令的散列值
  var md5 = crypto.createHash('md5');
  var password = md5.update(req.body.password).digest('base64');
  
  User.get(req.body.username, function(err, user) {
    if (!user) {
      req.flash('error', '用戶不存在');
      return res.redirect('/login');
    }
    if (user.password != password) {
      req.flash('error', '用戶口令錯誤');
      return res.redirect('/login');
    }
    req.session.user = user;
    req.flash('success', '登入成功');
    res.redirect('/');
  });
});

app.get('/logout', function(req, res) {
  req.session.user = null;
  req.flash('success', '登出成功');
  res.redirect('/');
});
