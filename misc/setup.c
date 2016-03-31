#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char **argv)
{
	char *ptr = getenv("EGG");
	if (ptr != NULL)
	{
		printf("Estimated address: %p\n", ptr);
		return 0;
	}
	printf("Setting up environment...\n");
	setenv("EGG", "/bin/sh", 1);
	execl("/bin/sh", (char *)NULL);
}
