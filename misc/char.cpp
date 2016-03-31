#include <stdio.h>
#include <string.h>

void fun(char c[])
{    printf("%d\n" , sizeof(c)); }

void fun2(char &c)
{    printf("%d\n" , sizeof(c)); }

void fun3(char(&c)[9])
{    printf("%d\n" , sizeof(c)); }

int main()
{
    char c[] = "12345678";//9,4,1,9
//    char *c = "12345678";

    printf("%d\n" , sizeof(c));
    fun(c);
    fun2(*c);
    fun3(c);
    return 0;
}

