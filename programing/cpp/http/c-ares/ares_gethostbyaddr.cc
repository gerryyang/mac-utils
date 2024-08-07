#include <time.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <ares.h>

void dns_callback (void* arg, int status, int timeouts, struct hostent* host)
{
	if(status == ARES_SUCCESS)
		std::cout << host->h_name << "\n";
	else
		std::cout << "lookup failed: " << status << '\n';
}
void main_loop(ares_channel &channel)
{
	int nfds, count;
	fd_set readers, writers;
	timeval tv, *tvp;
	while (1) {
		FD_ZERO(&readers);
		FD_ZERO(&writers);
		nfds = ares_fds(channel, &readers, &writers);
		if (nfds == 0)
			break;
		tvp = ares_timeout(channel, NULL, &tv);
		count = select(nfds, &readers, &writers, NULL, tvp);
		ares_process(channel, &readers, &writers);
	}

}
int main(int argc, char **argv)
{
	struct in_addr ip;
	int res;
	if(argc < 2 ) {
		std::cout << "usage: " << argv[0] << " ip.address\n";
		return 1;
	}
	inet_aton(argv[1], &ip);
	ares_channel channel;
	if((res = ares_init(&channel)) != ARES_SUCCESS) {
		std::cout << "ares feiled: " << res << '\n';
		return 1;
	}
	ares_gethostbyaddr(channel, &ip, sizeof ip, AF_INET, dns_callback, NULL);
	main_loop(channel);
	return 0;
}
