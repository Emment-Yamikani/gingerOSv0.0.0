#include "../include/string.h"

void swap(char *c, char *d)
{
	char temp = *c;
	*c = *d;
	*d = temp;
}

void reverse(char *str)
{
	int i = 0, j = strlen(str) - 1;
	for (; i < j;)
		swap(&str[i++], &str[j--]);
}

int memcmp(const void* aptr, const void* bptr, size_t size) {
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}
