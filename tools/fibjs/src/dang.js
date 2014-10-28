
var coroutine = require("coroutine");

while (true) {
	console.log('dang...');
	coroutine.sleep(1000);
}
/*
output:
gerryyang@mba:src$fibjs dang.js 
dang...
dang...
dang...
^C  
 */
