#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include "addressbook.pb.h"

std::string string_to_hex(const std::string& in) 
{
	std::stringstream ss;

	ss << std::hex << std::setfill('0');
	for (size_t i = 0; in.length() > i; ++i) {
		ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
	}

	return ss.str();
}

std::string hex_to_string(const std::string& in) 
{
	std::string output;

	if ((in.length() % 2) != 0) {
		throw std::runtime_error("String is not valid length ...");
	}

	size_t cnt = in.length() / 2;

	for (size_t i = 0; cnt > i; ++i) {
		uint32_t s = 0;
		std::stringstream ss;
		ss << std::hex << in.substr(i * 2, 2);
		ss >> s;

		output.push_back(static_cast<unsigned char>(s));
	}

	return output;
}

int main()
{
	tutorial::AddressBook address_book;
	address_book.add_people()->set_name("gerry");

	const int kBufSize = 1024;
	unsigned char abyBuf[kBufSize];

	google::protobuf::io::ArrayOutputStream stStream(abyBuf, kBufSize);
	if (!address_book.SerializeToZeroCopyStream(&stStream))
	{
		std::cout << "Encode(SerializeToZeroCopyStream) error\n";
		return 1;
	}
	auto uLen = (uint32_t)stStream.ByteCount();

	std::cout << "Encode(SerializeToZeroCopyStream) ok, uLen(" << uLen << ")\n";
	std::string strOrig((const char*)abyBuf, uLen);
	auto strHex = string_to_hex(strOrig);
	std::cout << "strHex(" << strHex << ")\n";

	if (!address_book.ParseFromArray(abyBuf, uLen))
	{
		std::cout << "Decode(ParseFromArray) error\n";
		return 1;
	}
	std::cout << "Decode(ParseFromArray) ok\n";

	return 0;
}
