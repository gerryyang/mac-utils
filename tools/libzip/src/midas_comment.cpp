#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <errno.h>
#include "zip.h"
using namespace std;

int encode_hex_string(const unsigned char *src, int len, unsigned char *dst)
{
	unsigned char szHexTable[] = "0123456789ABCDEF";

	for (int i = 0; i < len; ++i)
	{
		*dst = szHexTable[(src[i] >> 4) & 0x0f];
		++dst;
		*dst = szHexTable[src[i] & 0x0f];
		++dst;
	}
	*dst = '\0';

	return 0;
}

int main(int argc, char* argv[])
{

	if (argc < 3)
	{
		printf("usage: %s zipfile zipcomment\n", argv[0]);
		exit(1);
	}

	string zipfile = argv[1];
	string zipcomment = argv[2];
	int error;

	struct zip * zipfd = zip_open(zipfile.c_str(), ZIP_CHECKCONS, &error);
	if (zipfd == NULL)
	{
		switch (error)
		{
		case ZIP_ER_NOENT:
			printf("The file specified by path does not exist and ZIP_CREATE is not set [%d]\n", error);
			break;
		case ZIP_ER_EXISTS:
			printf("The file specified by path exists and ZIP_EXCL is set [%d]\n", error);
			break;
		case ZIP_ER_INVAL:
			printf("The path argument is NULL [%d]\n", error);
			break;
		case ZIP_ER_NOZIP:
			printf("The file specified by path is not a zip archive [%d]\n", error);
			break;
		case ZIP_ER_OPEN:
			printf("The file specified by path could not be opened [%d]\n", error);
			break;
		case ZIP_ER_READ:
			printf("A read error occurred; see errno for details [%d]\n", error);
			break;
		case ZIP_ER_SEEK:
			printf("The file specified by path does not allow seeks [%d]\n", error);
			break;
		default:
			printf("unknown err [%d]\n", error);
			break;
		}
		exit(1);
	}

	// get the comment for the entire zip archive
	int commentlen = 0;
	const char * comment = zip_get_archive_comment(zipfd, &commentlen, ZIP_FL_ENC_RAW);
	if (comment == NULL)
	{
		printf("zip_get_archive_comment get null or err[%d:%s]\n", errno, strerror(errno));
	}
	else 
	{
		printf("zip_get_archive_comment[%d:%s]\n", commentlen, comment);
		char copy[1024] = {0};
		memcpy(copy, comment, commentlen);
		unsigned char hex[1024] = {0};
		encode_hex_string((unsigned char *)copy, commentlen, hex);
		printf("zip_get_archive_comment hex[%d:%s]\n", commentlen, hex);
	}

	// Midas Header
	// idx:0 bytes:2 0X96FA
	// idx:2 bytes:2 comment len
	// idx:4 bytes:N channelId=xxx
	// idx:4+N bytes:2 end:0X0D0A
	char dstcomment[1024] = {0};
	int dstlen = 0;
	memset(dstcomment + dstlen, 0XFA, 1);
	dstlen += 1;
	memset(dstcomment + dstlen, 0X96, 1);
	dstlen += 1;
	memset(dstcomment + dstlen, zipcomment.length() + 2, 1);
	dstlen += 2;
	memcpy(dstcomment + dstlen, zipcomment.data(), zipcomment.length());
	dstlen += zipcomment.length();
	memset(dstcomment + dstlen, 0X0D, 1);
	dstlen += 1;
	memset(dstcomment + dstlen, 0X0A, 1);
	dstlen += 1;

	unsigned char hex[1024] = {0};
	encode_hex_string((unsigned char *)dstcomment, dstlen, hex);
	printf("zip_set_archive_comment hex[%d:%s]\n", dstlen, hex);


	// sets the comment for the entire zip archive
	// If comment is NULL and len is 0, the archive comment will be removed
	// comment must be encoded in ASCII or UTF-8
	int iret = zip_set_archive_comment(zipfd, (const char *)dstcomment, dstlen);// err !!!
	if (iret != 0)
	{
		printf("zip_set_archive_comment err[%d:%s]\n", errno, strerror(errno));
	}

	// close, If any files within were changed, those changes are written to disk first
	iret = zip_close(zipfd);
	if (iret != 0)
	{
		printf("zip_close err[%d:%s]\n", errno, strerror(errno));
	}

	return 0;
}
