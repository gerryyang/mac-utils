#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <errno.h>
#include "zip.h"
using namespace std;

int main(int argc, char* argv[])
{

	if (argc < 5)
	{
		printf("usage: %s zipfile entries entries_idx zipcomment\n", argv[0]);
		exit(1);
	}

	string zipfile = argv[1];
	string entries = argv[2];
	int entries_idx = atoi(argv[3]);
	string zipcomment = argv[4];
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

	// get the original number of entries
	zip_int64_t num = zip_get_num_entries(zipfd, ZIP_FL_UNCHANGED);
	if (num == -1)
	{
		printf("zip_get_num_entries err[%d:%s]\n", errno, strerror(errno));
	}
	else
	{
		printf("zip_get_num_entries[%lld]\n", num);
	}

	zip_int16_t cnt = zip_file_extra_fields_count(zipfd, entries_idx, ZIP_FL_UNCHANGED);
	if (cnt == -1)
	{
		printf("zip_file_extra_fields_count err[%d:%s]\n", errno, strerror(errno));
	}
	else
	{
		printf("zip_file_extra_fields_count[%d]\n", cnt);
	}

	// try find a file
	zip_int64_t idx = zip_name_locate(zipfd, entries.c_str(), ZIP_FL_ENC_GUESS);
	if (idx == -1)
		printf("find no [%lld:%s] in zip\n", idx, entries.c_str());
	else
		printf("find [%lld:%s] in zip\n", idx, entries.c_str());

	// get zip comment
	const char * comment = zip_file_get_comment(zipfd, entries_idx, NULL, ZIP_FL_ENC_RAW);
	if (comment == NULL)
	{
		printf("zip_file_get_comment get null or err[%d:%s]\n", errno, strerror(errno));
	}
	else 
	{
		printf("zip_file_get_comment[%s]\n", comment);
	}

	// set zip comment
	// If comment is NULL and len is 0, the file comment will be removed
	int iret = zip_file_set_comment(zipfd, entries_idx, zipcomment.c_str(), zipcomment.length(), ZIP_FL_ENC_GUESS);
	if (iret != 0)
	{
		printf("zip_file_set_comment err[%d:%s]\n", errno, strerror(errno));
	}
	else 
	{
		printf("zip_file_set_comment[%s]\n", zipcomment.c_str());
	}

	// entries state
	struct zip_stat info;
	//iret = zip_stat(zipfd, entries.c_str(), ZIP_FL_ENC_GUESS, &info);
	iret = zip_stat_index(zipfd, entries_idx, ZIP_FL_ENC_GUESS, &info);
	if (iret != 0)
	{
		printf("zip_stat err[%d:%s]\n", errno, strerror(errno));
	}
	else 
	{
		printf("zip_stat ok[%lld]\n", info.valid);

		if (info.valid & ZIP_STAT_NAME)
		{
			printf("name[%s]\n", info.name);
		}
		if (info.valid & ZIP_STAT_INDEX)
		{
			printf("index[%llu]\n", info.index);
		}
		if (info.size & ZIP_STAT_SIZE)
		{
			printf("size[%llu]\n", info.size);
		}
		if (info.mtime & ZIP_STAT_MTIME)
		{
			printf("mtime[%ld]\n", info.mtime);
		}
	}

	// get the comment for the entire zip archive
	int icommentlen = 0;
	comment = zip_get_archive_comment(zipfd, &icommentlen, ZIP_FL_ENC_RAW);
	if (comment == NULL)
	{
		printf("zip_get_archive_comment get null or err[%d:%s]\n", errno, strerror(errno));
	}
	else 
	{
		printf("zip_get_archive_comment[%d:%s]\n", icommentlen, comment);
	}

	// sets the comment for the entire zip archive
	// If comment is NULL and len is 0, the archive comment will be removed
	// comment must be encoded in ASCII or UTF-8
	iret = zip_set_archive_comment(zipfd, zipcomment.c_str(), zipcomment.length());
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
