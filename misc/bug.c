#include <stdio.h>
#include <string.h>
void bug(char *arg1)
{
	char name[128];
	strcpy(name, arg1);
	printf("Hello %s\n", name);
}
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <your name>\n", argv[0]);
		return 0;
	}
	bug(argv[1]);
	return 0;
}
