app.post('/reg', function(req, res) {
  //檢驗用戶兩次輸入的口令是否一致
  if (req.body['password-repeat'] != req.body['password']) {
    req.flash('error', '兩次輸入的口令不一致');
    return res.redirect('/reg');
  }

  //生成口令的散列值
  var md5 = crypto.createHash('md5');
  var password = md5.update(req.body.password).digest('base64');
  
  var newUser = new User({
    name: req.body.username,
    password: password,
  });
  
  //檢查用戶名是否已經存在
  User.get(newUser.name, function(err, user) {
    if (user)
      err = 'Username already exists.';
    if (err) {
      req.flash('error', err);
      return res.redirect('/reg');
    }
    //如果不存在則新增用戶
    newUser.save(function(err) {
      if (err) {
        req.flash('error', err);
        return res.redirect('/reg');
      }
      req.session.user = newUser;
      req.flash('success', '註冊成功');
      res.redirect('/');
    });
  });
});
