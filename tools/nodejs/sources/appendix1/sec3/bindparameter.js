var person = {
  name: 'byvoid',
  says: function(act, obj) {
    console.log(this.name + ' ' + act + ' ' + obj);
  }
};

person.says('loves', 'diovyb'); // 輸出 byvoid loves diovyb

byvoidLoves = person.says.bind(person, 'loves');
byvoidLoves('you'); // 輸出 byvoid loves you
