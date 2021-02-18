#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <net/if.h>

// 线程数
#define NTHREADS 10
// 生成psn数量
#define ITERATIONS 10000
// sleep毫秒数
#define SLEEP_MS 10
// 每生成多少psn休眠一次
#define SLEEP_PER_WORK_PER_THREAD 100

#define MY_SHM_ID 0x33b5

#define OK 0
#define FAIL -1

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
		close(s);
		return -1;
	}

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	for (i = 0; i < ifs; i++) {
		char ip[INET_ADDRSTRLEN];
		struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

		if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) {
			strcpy(pchErrMsg, "获取本机IP失败 inet_ntop失败");
			close(s);
			return -1;
		}

		if (strncmp(ip, "127.", strlen("127.")) == 0) {
			continue;
		}
		close(s);

		*ipValue = parseIPv4string(ip);
		return OK;
	}

	strcpy(pchErrMsg, "获取本机IP失败 无有效IP");
	close(s);
	return -1;
}

int GetTimeIPCntSuffix(char* suffix, int len, char* pchErrMsg, unsigned int &old_sn_bak)
{
	const static char s_char_array[] = {
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g',
		'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y', 'z'
	};

	// min(2**32 % 55**3, 55**3) = 163046
	const static char s_serial_array[] = {
		'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G',
		'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g',
		'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's'
	};

	size_t print_pos = 0;

	time_t t = time(NULL);
	struct tm *ptm = localtime(&t);
	print_pos += snprintf(suffix + print_pos, len - print_pos, "%c%c%c",
	                      s_char_array[ ptm->tm_hour ], s_char_array[ ptm->tm_min ], s_char_array[ ptm->tm_sec ]);

	unsigned ipValue;
	if (GetIPValue(&ipValue, pchErrMsg) != OK) {
		return FAIL;
	}

	char szTmp[10] = {0}, tmpPos = 0;
	while (tmpPos < 6) {
		tmpPos += snprintf(szTmp + tmpPos, sizeof(szTmp) - tmpPos, "%c",
		                   s_char_array[ ipValue % sizeof(s_char_array) ]);
		ipValue /= sizeof(s_char_array);
	}
	for (int i = 0; i < 6; ++i) {
		suffix[print_pos + i] = szTmp[6 - 1 - i];
	}
	print_pos += 6;

	int shmid = shmget(MY_SHM_ID, sizeof(int), IPC_CREAT | 0666);
	if (shmid == -1) {
		printf("shmget failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		sprintf(pchErrMsg, "shmget failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return FAIL;
	}
	unsigned int * p_sn = (unsigned int *)shmat(shmid, NULL, 0);
	if (p_sn == (unsigned int *) - 1) {
		printf("shmat failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		sprintf(pchErrMsg, "shmat failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return FAIL;
	}
	unsigned old_sn = __sync_fetch_and_add(p_sn, 1);
	if (shmdt(p_sn) == -1) {
		printf("shmdt failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		sprintf(pchErrMsg, "shmdt failed, %s(%d,id=%x)", strerror(errno), errno, MY_SHM_ID);
		return FAIL;
	}
	//printf("%d\n", old_sn);
	old_sn_bak = old_sn;

	// 55 ^ 3
	old_sn %= (sizeof(s_serial_array) * sizeof(s_serial_array) * sizeof(s_serial_array));

	// 3位
	tmpPos = 0;
	while (tmpPos < 3) {
		tmpPos += snprintf(szTmp + tmpPos, sizeof(szTmp) - tmpPos, "%c",
		                   s_serial_array[ old_sn % sizeof(s_serial_array) ]);
		old_sn /= sizeof(s_serial_array);
	}
	for (int i = 0; i < 3; ++i) {
		suffix[print_pos + i] = szTmp[3 - 1 - i];
	}
	suffix[print_pos + 3] = 0;

	return OK;
}

void* thread_run(void* arg)
{
	int i, totalIterations = ITERATIONS / NTHREADS;
	char pErrMsg[512] = {0}, szSuffix[20], szPSN[512];
	unsigned int old_sn = 0;
	for (i = 1; i <= totalIterations; ++i) {

		GetTimeIPCntSuffix(szSuffix, sizeof(szSuffix), pErrMsg, old_sn);
#if 1
		snprintf(szPSN, sizeof(szPSN), "%s\n", szSuffix);
#else
// debug
		snprintf(szPSN, sizeof(szPSN), "%s - %u\n", szSuffix, old_sn);
#endif
		printf("%s", szPSN);

		if (SLEEP_MS > 0 && i % SLEEP_PER_WORK_PER_THREAD == 0) {
			usleep(SLEEP_MS);
		}
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
