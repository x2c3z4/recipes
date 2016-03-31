#include <stdio.h>
#include <stdint.h>

uint64_t fibonacci(unsigned int n){
	if(n > 1){
		uint64_t a,b=1;
		n--;
		a = n & 1;
		n /= 2;
		while(n-- > 0){
			a += b;
			b += a;
		}
		return b;
	}
	return n;
}

int main()
{
	int n;
	scanf("%d",&n);
	printf("f(%d)=%llu\n",n,fibonacci(n));
	return 0;
}
