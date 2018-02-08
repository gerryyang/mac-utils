app.get('/', function(req, res) {
  Post.get(null, function(err, posts) {
    if (err) {
      posts = [];
    }
    res.render('index', {
      title: '首頁',
      posts: posts,
    });
  });
});
