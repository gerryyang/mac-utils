#ifndef MSGPACK_CLIENT_H_
#define MSGPACK_CLIENT_H_

#include <string>
#include <deque>

#include "msgpack.hpp"

typedef struct 
{
	int 	len;
	int 	ver;
	int 	seqno;
	int 	resver0;
	int 	resver1;
	char 	data[0];
} mp_pkt_t;

typedef struct ERROR_STACK_T
{
	int code;
	std::string msg;
	std::string source;
	std::string display;
	std::string fileinfo;
	short type;
	short weight;

	int metric;
	int resver;

	std::string service;
	std::string func;
	ERROR_STACK_T() :code(0), type(0), weight(0), metric(0), resver(0){}
	ERROR_STACK_T(int _code, std::string _msg, std::string _fileinfo) :type(0), weight(0), metric(0), resver(0)
	{
		code     =  _code;
		msg      =  _msg;
		fileinfo =  _fileinfo;
	}
	MSGPACK_DEFINE(code, msg, source, display, type, weight, metric, resver, service, func);
} error_stack_t;

class body_t
{
public:
	std::string uuid;
	int metric;
	int resver;

	std::string service;
	std::string func;
	std::string sig;

	std::deque<error_stack_t> err_vec;

	std::map<std::string, std::string> rec_map;
	std::map<std::string, std::map<std::string, std::string> > rec_nmap;
	std::map<std::string, std::vector<std::string> > rec_vec;

	body_t() :metric(0){}
public:
	MSGPACK_DEFINE(uuid, metric, resver, service, func, sig, err_vec, rec_map, rec_nmap, rec_vec);
};

typedef struct protocol_s
{
	int header[5]; //{len, ver, seqno, resver1, resver2};
	body_t body;
	protocol_s() { memset(header, 0, sizeof(header)); }
} protocol_t;


std::map<std::string, std::string> m;

class Client
{
public:
	Client();
	~Client();

	int set_address(const std::string& ip, unsigned short port);
	int set_timeout(int timeout);

	int request(const protocol_t& req, protocol_t& rsp, std::string& err);

	int request_http(const std::string& req, std::string& rsp, std::string& err);

	int close();

private:
	int connect();

	int encode(const protocol_t& req);

	int send_packet(const char* buf, int len);
	int recv_packet(int len);

	int recv_http(std::string& rsp);

private:
	std::string m_ip;
	unsigned short m_port;
	int m_timeout;

	int m_netfd;

	// 数据
	char* m_data;         // 数据
	int   m_data_size;    // 最大长度
	int   m_data_length;  // 使用长度

	static int g_sn;

	Client(const Client&);
	void operator = (const Client&);
};

#endif // MSGPACK_CLIENT_H_
