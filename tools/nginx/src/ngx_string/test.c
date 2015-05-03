#include <stdio.h>
#include <sys/types.h>

#define ngx_tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define ngx_toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

int main()
{
	char c1 = 'A';
	char c2 = 'a';

	printf("%c\n", ngx_tolower(c1));// a
	printf("%c\n", ngx_toupper(c2));// A

	return 0;
}
