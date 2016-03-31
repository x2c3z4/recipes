#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
        char buffer[96];

        printf("- %p -//n", &buffer);
        strcpy(buffer, getenv("KIRIKA"));

        return 0;
}
