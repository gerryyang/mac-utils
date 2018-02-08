function Person() {
}
Person.prototype.name = 'BYVoid';
Person.prototype.showName = function () {
  console.log(this.name);
};

var person = new Person();
person.showName();
