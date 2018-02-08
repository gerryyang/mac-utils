module.exports = function(app) {
  app.get('/', function(req, res) {
    res.render('index', {
      title: '首頁'
    });
  });
  
  app.get('/reg', function(req, res) {
    res.render('reg', {
      title: '用戶註冊',
    });
  });
};
