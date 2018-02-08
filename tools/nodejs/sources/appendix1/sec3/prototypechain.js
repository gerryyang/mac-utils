function Foo() {
}
Object.prototype.name = 'My Object';
Foo.prototype.name = 'Bar';

var obj = new Object();
var foo = new Foo();
console.log(obj.name); // 輸出 My Object
console.log(foo.name); // 輸出 Bar
console.log(foo.__proto__.name); // 輸出 Bar
console.log(foo.__proto__.__proto__.name); // 輸出 My Object
console.log(foo.__proto__.constructor.prototype.name); // 輸出 Bar
