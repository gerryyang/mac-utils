/*
   利用协程，我们可以完成很多有趣的事情，也可以用更有趣的方式完成事情。在很多时候，我们需要在一次请求完成多个独立的数据库操作，而且大部分时候，这些操作之间并无制约关系。通过协程，我们可以很方便地将这些操作平行发送，以提高响应速度。
   上述代码中的多个匿名函数，会被同时执行，并在全部完成后，继续下面的代码。
   */

var coroutine = require("coroutine");

function func(){

	console.log("let's go");

	coroutine.parallel(function(){
		console.log('dang1...');
	}, function(){
		console.log('dang2...');
	}, function(){
		console.log('dang3...');
	}, function(){
		console.log('dang4...');
	});
}

func();

/*
output:
gerryyang@mba:src$fibjs coroutine.js 
let's go
dang1...
dang2...
dang3...
dang4...
 */

