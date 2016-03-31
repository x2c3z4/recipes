#include <stdio.h>

void fun1(int *p,int *q)
{
	int c;
	c=*p;
	*p=*q;
	*q=c;
}

void fun2(int *p,int *q)
{
	int *c;
	c=p;
	p=q;
	q=c;
}

void test1()
{
	int x=3,y=4;
	int *p=&x,*q=&y;
	printf("before swap\n");
	printf("x=%d,y=%d,&x=%p,&y=%p\n",x,y,&x,&y);
	printf("*p=%d,*q=%d,p=%p,q=%p\n",*p,*q,p,q);
	fun1(p,q);
	printf("after swap\n");
	printf("x=%d,y=%d,&x=%p,&y=%p\n",x,y,&x,&y);
	printf("*p=%d,*q=%d,p=%p,q=%p\n",*p,*q,p,q);
}

void test2()
{
	int x=3,y=4;
	int *p=&x,*q=&y;
	printf("before swap\n");
	printf("x=%d,y=%d,&x=%p,&y=%p\n",x,y,&x,&y);
	printf("*p=%d,*q=%d,p=%p,q=%p\n",*p,*q,p,q);
	fun2(p,q);
	printf("after swap\n");
	printf("x=%d,y=%d,&x=%p,&y=%p\n",x,y,&x,&y);
	printf("*p=%d,*q=%d,p=%p,q=%p\n",*p,*q,p,q);
}


int main()
{
	test1();
	printf("--------------------------------------------\n");
	test2();
	return 0;
}
