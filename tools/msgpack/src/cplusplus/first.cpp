#include <msgpack.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>

int main(void) 
{
	// serializes this object.
	std::vector<std::string> vec;
	vec.push_back("Hello");
	vec.push_back("MessagePack");

	// serialize it into simple buffer.
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, vec);

	// deserialize it.
	msgpack::unpacked msg;
	msgpack::unpack(&msg, sbuf.data(), sbuf.size());

	// print the deserialized object.
	msgpack::object obj = msg.get();
	std::cout << obj << std::endl;  //=> ["Hello", "MessagePack"]

	// convert it into statically typed object.
	std::vector<std::string> rvec;
	obj.convert(&rvec);

	std::cout << "convert it into statically typed object." << std::endl;
	std::vector<std::string>::iterator it = rvec.begin();
	for (; it != rvec.end(); ++it )
	{
		std::cout << *it << std::endl;
	}

	std::map<std::string, std::string> m;
	m["a"] = "1";
	m["b"] = "2";

	msgpack::sbuffer sbuf2;
	msgpack::pack(sbuf2, m);
	msgpack::unpacked msg2;
	msgpack::unpack(&msg2, sbuf2.data(), sbuf2.size());
	msgpack::object obj2 = msg2.get();
	std::cout << obj2 << std::endl;

	std::map<std::map<std::string, std::string>, std::string> m2;
	m2[m] = "3";
	msgpack::sbuffer sbuf3;
	msgpack::pack(sbuf3, m2);
	msgpack::unpacked msg3;
	msgpack::unpack(&msg3, sbuf3.data(), sbuf3.size());
	msgpack::object obj3 = msg3.get();
	std::cout << obj3 << std::endl;

	return 0;
}
/*
["Hello", "MessagePack"]
convert it into statically typed object.
Hello
MessagePack
{"a"=>"1", "b"=>"2"}
{{"a"=>"1", "b"=>"2"}=>"3"}
*/
