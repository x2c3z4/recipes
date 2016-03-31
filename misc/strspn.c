#include <string.h>
#include <stdio.h>

#ifndef NDEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

int main()
{
	int len;
	len = strcspn("adcd    a b c"," ");
	DBG("len=%d\n",len);
}
