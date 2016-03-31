#include <stdio.h>

int main()
{
	char a[] = {0x10,0x12,0x13,0x14,0x15,0x16};
	char *str=a;
	printf("0x%X\n",*(str+3));
	printf("0x%x\n",*(int *)str);
	printf("0x%X\n",*(short *)str);
	printf("0x%X\n",*(short *)(str+2));
	printf("0x%X\n",*(short *)(str+4));
	return 0;
}

