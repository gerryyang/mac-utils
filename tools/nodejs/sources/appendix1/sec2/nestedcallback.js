exports.add_user = function(user_info, callback) {
  var uid = parseInt(user_info['uid']);
  mongodb.open(function(err, db) {
    if (err) {callback(err); return;}
    db.collection('users', function(err, collection) {
      if (err) {callback(err); return;}
      collection.ensureIndex("uid", function(err) {
        if (err) {callback(err); return;}
        collection.ensureIndex("username", function(err) {
          if (err) {callback(err); return;}
          collection.findOne({uid: uid}, function(err) {
            if (err) {callback(err); return;}
            if (doc) {
              callback('occupied');
            } else {
              var user = {
                uid: uid,
                user: user_info,
              };
              collection.insert(user, function(err) {
                callback(err);
              });
            }
          });
        });
      });
    });
  });
};
