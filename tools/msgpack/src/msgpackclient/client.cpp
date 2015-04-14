#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "client.h"

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ((nread = read(fd, ptr, nleft)) < 0) 
		{
			if (errno == EINTR)
			{
				nread = 0;		/* and call read() again */
			}
			else if (errno == EAGAIN)
			{
				//std::cout << "read result: try again" << std::endl;
				usleep(1);
				nread = 0;
			}
			else
			{
				std::cout << "read result:" << errno << "," << strerror(errno) << std::endl;
				return(-1);
			}
		}
		else if (nread == 0)
		{
			std::cout << "read result close" << std::endl;
			break;				/* EOF */
		}

		nleft -= nread;
		ptr += nread;
	}

	return(n - nleft);		/* return >= 0 */
}

int Client::g_sn = 10;

const int MSG_PACK_VER = 10001;
const int MSG_PACK_HEAD_LEN = 20;

Client::Client()
: m_ip("")
, m_port(0)
, m_timeout(0)
, m_netfd(-1)
{
	m_data_size = 10 * 1024;
	m_data = (char*)malloc(m_data_size);	  // 数据
	m_data_length = 0;	  // 使用长度
}

Client::~Client()
{
	if (NULL != m_data)
	{
		free(m_data);
	}
}

int Client::set_address(const std::string& ip, unsigned short port)
{
	m_ip   =  ip;
	m_port =  port;

	return 0;
}

int Client::set_timeout(int timeout)
{
	if (m_timeout != timeout)
	{
		// 重新连接
		close();
	}

	m_timeout = timeout;
	return 0;
}

int Client::request_http(const std::string& req, std::string& rsp, std::string& err)
{
	int ret = send_packet(req.c_str(), req.length());
	if (ret != 0)
	{
		close();
		std::cout << "send_packet failed: " << ret << std::endl;
		return ret;
	}

	ret = recv_http(rsp);

	return ret;

}

int Client::request(const protocol_t& req, protocol_t& rsp, std::string& err)
{
	int ret = encode(req);
	if (ret != 0) return ret;

	//std::cout << "req data length: " << m_data_length << std::endl;

	ret = send_packet(m_data, m_data_length);
	if (ret != 0)
	{
		close();
		std::cout << "send_packet failed: " << ret << std::endl;
		return ret;
	}


	//std::cout << "read begin:" << getpid() << std::endl;
	// 解析头文件
	int header_len = MSG_PACK_HEAD_LEN;
	int nread = readn(m_netfd, m_data, header_len);
	if (nread != header_len)
	{
		return -10;
	}

	//std::cout << "read end:" << getpid() << std::endl;

	int pack_len = htonl(*(const int*)(m_data));
	int ver = htonl(*(const int*)(m_data + 4));
	int seq = htonl(*(const int*)(m_data + 8));

	ret = recv_packet(pack_len - header_len);
	if (ret != 0) return ret;

	rsp.header[0] = pack_len;
	rsp.header[1] = ver;
	rsp.header[2] = seq;

	msgpack::unpacked msg;
	try
	{
		msgpack::unpack(&msg, m_data + MSG_PACK_HEAD_LEN, pack_len - header_len);
		msgpack::object obj = msg.get();
		//std::cout << " obj = " << obj << std::endl;
		obj.convert(&rsp.body);
	}
	catch (...)
	{
		return 100;
	}

	return 0;
}

int Client::connect()
{
	if (m_netfd >= 0) return 0;
	int err = -1;  


	m_netfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_netfd < 0)
	{
		err = errno;
		return err;
	}

	int sec = m_timeout / 1000;
	int usec = (m_timeout % 1000) * 1000;
	struct timeval tv = { sec, usec };
	setsockopt(m_netfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	//if (m_timeout) tv.tv_usec = 100000; // add more 100ms
	if (m_timeout)
	{
		tv.tv_sec = (m_timeout + 100) / 1000;
		tv.tv_usec = ((m_timeout + 100) % 1000) * 1000;
	}
	setsockopt(m_netfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	struct sockaddr_in	servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
	servaddr.sin_port = htons(m_port);

	int flag = fcntl(m_netfd, F_GETFL, 0);
	if (flag & O_NONBLOCK)
	{
		std::cout << "flag has O_NONBLOCK" << std::endl;
	}

	int i = 1;
	setsockopt(m_netfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

	if (::connect(m_netfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
	{
		//std::cout << int(getpid()) << "connect ok" << std::endl;
		return 0;
	}

	err = errno;

	std::cout << int(getpid()) << "connect failed: " << errno << ", " << strerror(errno) << std::endl;

	close();

	return err;
}

int Client::close()
{
	if (-1 != m_netfd)
	{
		::close(m_netfd);
		m_netfd = -1;

		//std::cout << int(getpid()) << "connect close : " << errno << ", " << strerror(errno) << std::endl;
	}

	return 0;
}

int Client::send_packet(const char* buf, int len)
{
	int err;
	if (m_netfd < 0)
	{
		err = connect();
		if (err != 0)
		{
			return err;
		}
	}
	/*
	   else
	   {
	   char tmp[1];
	   err = recv(m_netfd, tmp, sizeof(tmp), MSG_PEEK | MSG_DONTWAIT);
	   if (err == 0 || (err<0 && errno != EAGAIN && errno != EWOULDBLOCK))
	   {
	   std::cout << "recv failed " << err << std::endl;
	   close();
	   err = connect();
	   if (err != 0)
	   {
	   std::cout << "connect failed:" << err << std::endl;
	   return err;
	   }
	   }
	   }
	   */
	size_t nleft = len;
	ssize_t nwritten;
	const char *ptr = buf;
	while (nleft > 0)
	{
		if ((nwritten = write(m_netfd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nwritten = 0;
			}
			else if (errno == EAGAIN)
			{
				std::cout << "write EAGAIN:" << getpid() << std::endl;
				nwritten = 0;
			}
			else
			{
				std::cout << "write failed:" << errno << "," << strerror(errno) << std::endl;
				return (0 - errno);
			}
		}
		else if (nwritten == 0)
		{
			std::cout << "write 0:" << getpid() << std::endl;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return 0;
}

int Client::recv_packet(int len)
{
	if (m_netfd < 0)
		return -1;

	int nread = 0;
	if (len > 0)
	{
		if ((nread = readn(m_netfd, m_data + MSG_PACK_HEAD_LEN, len)) < 0)
			return errno;
	}

	return 0;
}

int Client::recv_http(std::string& rsp)
{
	char buf[1024] = {0};

	char* ptr = (char*)buf;
	int nleft = 1024 - 1;
	int nread = 0;
	while (true)
	{
		if ((nread = read(m_netfd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nread = 0;		/* and call read() again */
			}
			else if (errno == EAGAIN)
			{
				//std::cout << "read result: try again" << std::endl;
				nread = 0;
			}
			else
			{
				std::cout << "read result:" << errno << "," << strerror(errno) << std::endl;
				return(-1);
			}
		}
		else if (nread == 0)
		{
			std::cout << "read result close" << std::endl;
			break;				/* EOF */
		}

		nleft -= nread;
		ptr += nread;

		//std::cout << strlen(buf) << std::endl;
		if (strstr(buf, "\r\n\r\n") != NULL)
		{
			break;
		}
	}

	rsp = buf;

	return 0;
}

int Client::encode(const protocol_t& req)
{
	msgpack::sbuffer sbuf;

	msgpack::pack(&sbuf, req.body);
	int pack_len = sbuf.size() + MSG_PACK_HEAD_LEN;

	if (pack_len > m_data_size)
	{
		return -6;  // data error
	}

	mp_pkt_t *pkt = (mp_pkt_t *)(m_data);
	pkt->len = htonl(pack_len);
	pkt->ver = htonl(MSG_PACK_VER);
	pkt->seqno = htonl(g_sn++);
	if (g_sn == 0)
	{
		g_sn++;
	}
	memcpy(m_data + MSG_PACK_HEAD_LEN, sbuf.data(), sbuf.size());
	m_data_length = pack_len;

	return 0;
}

#define MAX_CLIENT 255

int test_http()
{
	int succ_cnt = 0;
	int fail_cnt = 0;

	Client client;
	client.set_address(std::string("10.151.135.210"), 30000);
	client.set_timeout(1000);

	std::string req = "GET /v1/qzone/tdftest?appid=1000000673 HTTP/1.1\r\n\r\n";
	std::string rsp;
	std::string error;

	timeval begin_time;
	gettimeofday(&begin_time, NULL);

	do
	{
		rsp = "";
		int ret = client.request_http(req, rsp, error);
		if (ret == 0)
		{
			if (rsp.find("HTTP/1.1 200 OK") != -1)
			{
				succ_cnt++;
				//std::cout << "handle ok" << std::endl;
			}
			else
			{
				fail_cnt++;
				client.close();

				std::cout << "handle failed" << std::endl;
			}
		}

		if ((succ_cnt + fail_cnt) >= 10) break;

	} while (true);

	timeval end_time;
	gettimeofday(&end_time, NULL);

	int usec = (end_time.tv_sec - begin_time.tv_sec) * 1000000 + end_time.tv_usec - begin_time.tv_usec;

	std::cout << "count:" << (succ_cnt + fail_cnt) << "\n"
		<< "succ: " << succ_cnt << "\n"
		<< "fail: " << fail_cnt << "\n"
		<< " time: " << usec << "\n"
		<< " count/sec:" << ((succ_cnt + fail_cnt) * 1000) / (double(usec) / 1000) << std::endl;

	return 0;
}

int test_msgpack()
{
	char m_data[4096];
	int m_data_size = 4096;
	int g_sn = 0;

	int n = 0;

	timeval begin_time;
	gettimeofday(&begin_time, NULL);

	do
	{
		protocol_t req;
		req.body.uuid    =  "0000-1111-2222-3333";
		req.body.metric  =  245573;
		req.body.service =  "tdftest";
		req.body.func    =  "test";
		req.body.sig     =  "why?";

		error_stack_t err_st;
		err_st.code =  10001;
		err_st.msg  =  "client error haha";
		req.body.err_vec.push_back(err_st);

		req.body.rec_map["test1"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test2"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test3"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test4"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test5"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test6"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test7"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test8"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test9"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		req.body.rec_map["test10"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

		// msgpack encode
		msgpack::sbuffer sbuf;

		msgpack::pack(&sbuf, req.body);
		int pack_len = sbuf.size() + MSG_PACK_HEAD_LEN;

		if (pack_len > m_data_size)
		{
			std::cout << "data buffer length is small" << std::endl;
			return -6;  // data error
		}

		mp_pkt_t *pkt =  (mp_pkt_t *)(m_data);
		pkt->len      =  htonl(pack_len);
		pkt->ver      =  htonl(MSG_PACK_VER);
		pkt->seqno    =  htonl(g_sn++);

		if (g_sn == 0)
		{
			g_sn++;
		}
		memcpy(m_data + MSG_PACK_HEAD_LEN, sbuf.data(), sbuf.size());

		// msgpack decode
		msgpack::unpacked msg;
		protocol_t rsp;
		try
		{
			msgpack::unpack(&msg, m_data + MSG_PACK_HEAD_LEN, pack_len - MSG_PACK_HEAD_LEN);
			msgpack::object obj = msg.get();
			//std::cout << " obj = " << obj << std::endl;
			obj.convert(&rsp.body);
		}
		catch (...)
		{

			std::cout << "unpacked failed" << std::endl;
			return -7;  // data error
		}

		n++;
		if (n >= 10000000) break;
	} while (true);

	timeval end_time;
	gettimeofday(&end_time, NULL);

	int usec = (end_time.tv_sec - begin_time.tv_sec) * 1000000 + end_time.tv_usec - begin_time.tv_usec;

	std::cout << "count:" << (n) << "\n"
		<< " time: " << usec << "\n"
		<< " count/sec:" << double(n) / (double(usec) / 1000000.0) << std::endl;

	return 0;
}

int main(int argc, char* argv[])
{
	return test_msgpack();
	//return test_http();

	int succ_cnt = 0;
	int fail_cnt = 0;

	timeval begin_time;

	gettimeofday(&begin_time, NULL);

	timeval step_begin_time = begin_time;
	timeval stop_end_time;

	Client client;
	client.set_address(std::string("10.151.135.210"), 30000);
	client.set_timeout(1000);

	do 
	{
		protocol_t req;
		protocol_t rsp;

		req.body.uuid    =  "0000-1111-2222-3333";
		req.body.metric  =  245573;
		req.body.service =  "tdftest";
		req.body.func    =  "test";
		req.body.sig     =  "why?";

		error_stack_t err_st;
		err_st.code =  10001;
		err_st.msg  =  "client error haha";
		req.body.err_vec.push_back(err_st);

		req.body.rec_map["test1"] = "1111";
		req.body.rec_map["test2"] = "2222";


		std::string str_err;
		int ret = client.request(req, rsp, str_err);
		if (ret == 0)
		{
			succ_cnt++;
			//std::cout << "handle OK:" << int(getpid()) << std::endl;
			// recv info
			/*
			   std::cout << "rsp.body.rec_map: " << std::endl;
			   std::map<std::string, std::string>::const_iterator it = rsp.body.rec_map.begin();
			   std::map<std::string, std::string>::const_iterator eit = rsp.body.rec_map.end();
			   for (; it != eit; ++it)
			   {
			   std::cout << it->first << ":" << it->second << std::endl;
			   }
			   */
			//std::cout << "handle success!" << std::endl;
			/*
			   std::cout << "rsp.body.err_vec size:" << rsp.body.err_vec.size() << std::endl;



			   std::cout << "rsp.body.err vec: " << std::endl;
			   while (rsp.body.err_vec.size() > 0)
			   {
			   std::cout << "code:" << rsp.body.err_vec.front().code << ", msg:" << rsp.body.err_vec.front().msg << std::endl;
			   rsp.body.err_vec.pop_front();
			   }
			   */
		}
		else
		{
			fail_cnt++;
			std::cout << "handle error:" << int(getpid()) << std::endl;
		}

		if (((succ_cnt + fail_cnt) % 100) == 0)
		{
			gettimeofday(&stop_end_time, NULL);
			if (stop_end_time.tv_sec > step_begin_time.tv_sec)
			{
				gettimeofday(&stop_end_time, NULL);
				int usec = (stop_end_time.tv_sec - begin_time.tv_sec) * 1000000 + stop_end_time.tv_usec - begin_time.tv_usec;
				std::cout << getpid() << " " << ((succ_cnt + fail_cnt) * 1000) / (double(usec) / 1000) << std::endl;

				step_begin_time = stop_end_time;
			}
		}

		client.close();
		/*
		   if (((succ_cnt + fail_cnt) % 10000) == 0)
		   {
		   gettimeofday(&stop_end_time, NULL);
		   int usec = (stop_end_time.tv_sec - step_begin_time.tv_sec) * 1000000 + stop_end_time.tv_usec - step_begin_time.tv_usec;
		   std::cout << getpid() << " " << ((succ_cnt + fail_cnt) * 1000) / (double(usec) / 1000) << std::endl;
		//succ_cnt = 0;
		//fail_cnt = 0;
		//step_begin_time = stop_end_time;
		}
		*/
		//if (succ_cnt + fail_cnt >= 10) break;
	} while (true);

	timeval end_time;
	gettimeofday(&end_time, NULL);

	int usec = (end_time.tv_sec - begin_time.tv_sec) * 1000000 + end_time.tv_usec - begin_time.tv_usec;

	std::cout << "count:" << (succ_cnt + fail_cnt) << "\n" 
		<< "succ: " << succ_cnt << "\n" 
		<< "fail: " << fail_cnt << "\n"
		<< " time: " << usec << "\n"
		<< " count/sec:" << ((succ_cnt + fail_cnt) * 1000) / (double(usec) / 1000)  << std::endl;

	return 0;
}

