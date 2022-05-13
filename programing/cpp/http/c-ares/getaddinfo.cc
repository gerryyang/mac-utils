#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(void)
{
	struct addrinfo* addr;
	int result = getaddrinfo("google.com", NULL, NULL, &addr);
	if (result != 0) {
		printf("Error from getaddrinfo: %s\n", gai_strerror(result));
		return 1;
	}
	struct sockaddr_in* internet_addr = (struct sockaddr_in*) addr->ai_addr;
	printf("google.com is at: %s\n", inet_ntoa(internet_addr->sin_addr));
	return 0;
}
