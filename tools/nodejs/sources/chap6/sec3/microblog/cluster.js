var cluster = require('cluster');
var os = require('os');

// 獲取 CPU 的數量
var numCPUs = os.cpus().length;

var workers = {};
if (cluster.isMaster) {
  // 主進程分支
  cluster.on('death', function (worker) {
    // 當一個工作進程結束時，重啓工作進程
    delete workers[worker.pid];
    worker = cluster.fork();
    workers[worker.pid] = worker;
  });
  // 初始開啓與 CPU 數量相同的工作進程
  for (var i = 0; i < numCPUs; i++) {
    var worker = cluster.fork();
    workers[worker.pid] = worker;
  }
} else {
  // 工作進程分支，啓動服務器
  var app = require('./app');
  app.listen(3000);
}

// 當主進程被終止時，關閉所有工作進程
process.on('SIGTERM', function () {
  for (var pid in workers) {
    process.kill(pid);
  }
  process.exit(0);
});
