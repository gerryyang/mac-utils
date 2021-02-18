#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define NTHREADS 1
#define ITERATIONS 50
#define SLEEP_MS 0
#define SLEEP_PER_WORK_PER_THREAD 1000

#define PAYPORTAL_OK 0
#define PORTAL_FAIL -1

unsigned parseIPv4string(char* ipaddr)
{
	int ipbytes[4] = {0};
	sscanf(ipaddr, "%d.%d.%d.%d", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
	return ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
}

int GetIPValue(unsigned* ipValue, char* pchErrMsg)
{
	int s;
	struct ifconf ifconf;
	struct ifreq ifr[50];
	int ifs;
	int i;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		strcpy(pchErrMsg, "获取本机IP失败 创建socket失败");
		return -1;
	}

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof ifr;

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
		strcpy(pchErrMsg, "获取本机IP失败 ioctl失败");
		return -1;
	}

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	for (i = 0; i < ifs; i++) {
		char ip[INET_ADDRSTRLEN];
		struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

		if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) {
			strcpy(pchErrMsg, "获取本机IP失败 inet_ntop失败");
			return -1;
		}

		if ( strncmp(ip, "127.", strlen("127.")) == 0 ) {
			continue;
		}
		close(s);

		*ipValue = parseIPv4string(ip);
		return PAYPORTAL_OK;
	}
	close(s);
	
	strcpy(pchErrMsg, "获取本机IP失败 无有效IP");
	return -1;
}

int InitRandomSeed(char* pchErrMsg)
{
	pid_t pid = getpid();
	struct timeval tm;
	gettimeofday(&tm, NULL);

	unsigned ipValue;
	if (GetIPValue(&ipValue, pchErrMsg) != PAYPORTAL_OK)
	{
		return PORTAL_FAIL;
	}

	unsigned ss = (unsigned)(pid ^ tm.tv_sec ^ tm.tv_usec ^ ipValue);
	srandom(ss);
	sprintf(pchErrMsg, "| %u %u %u %u | %u", pid, tm.tv_sec, tm.tv_usec, ipValue, ss);

	return PAYPORTAL_OK;
}

int GetRandomSuffix(char* suffix, int len, char* pchErrMsg)
{
	static bool inited = false;
	if (!inited) {
		if (InitRandomSeed(pchErrMsg) != PAYPORTAL_OK)
		{
			return PORTAL_FAIL;
		}
		inited = true;
	}

	const static char s_rand_char_array[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g',
		'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z',
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9'};

	for (int loop = 0; loop < 6; ++loop)
	{
		suffix[loop] = s_rand_char_array[random()%sizeof(s_rand_char_array)];
	}
	suffix[6] = '\0';

	return PAYPORTAL_OK;
}

void* thread_run(void* arg)
{
	int i, totalIterations = ITERATIONS / NTHREADS;
	char pErrMsg[512] = {0}, szRandomSuffix[10], szPSN[512];
	for (i = 1; i <= totalIterations; ++i){
		
		GetRandomSuffix(szRandomSuffix, 10, pErrMsg);

		time_t t = time(NULL);
		struct tm *ptm=localtime(&t);
		//snprintf(szPSN, sizeof(szPSN), "%02d%02d%02d%s || %s\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, szRandomSuffix, pErrMsg);
		snprintf(szPSN, sizeof(szPSN), "%02d%02d%02d%s\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, szRandomSuffix);
		printf("%s", szPSN);

		if (SLEEP_MS > 0 && i%SLEEP_PER_WORK_PER_THREAD == 0) 
			usleep(SLEEP_MS);
	}
}

int main(int argc, char** argv)
{

	pthread_t* thread_ids;
	thread_ids = (pthread_t*)calloc(NTHREADS, sizeof(pthread_t));
	/* create threads */
	int t = 0;
	for (t = 0; t < NTHREADS; t++) {
		pthread_create(&thread_ids[t], NULL, &thread_run, NULL);
	}
	for (t = 0; t < NTHREADS; t++) {
		pthread_join(thread_ids[t], NULL);
	}
	free(thread_ids);

	return 0;
}
