function User(name, uri) {
  this.name = name;
  this.uri = uri;
  this.display = function() {
    console.log(this.name);
  }
}
