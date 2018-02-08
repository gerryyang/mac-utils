app.post('/post', checkLogin);
app.post('/post', function(req, res) {
  var currentUser = req.session.user;
  var post = new Post(currentUser.name, req.body.post);
  post.save(function(err) {
    if (err) {
      req.flash('error', err);
      return res.redirect('/');
    }
    req.flash('success', '發表成功');
    res.redirect('/u/' + currentUser.name);
  });
});
