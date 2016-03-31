#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
        if(argc != 2)
        {
                printf("Usage: %s string\n", argv[0]);
                return 0;
        }
        
        unsigned long phymem_addr, phymem_size;
        char *map_addr;
        char s[256];
        int fd;
        
        /*get the physical address of allocated memory in kernel*/
        fd = open("/proc/memshare/phymem_addr", O_RDONLY);
        if(fd < 0)
        {
                printf("cannot open file /proc/memshare/phymem_addr\n");
                return 0;
        }
        read(fd, s, sizeof(s));
        sscanf(s, "%lx", &phymem_addr);
        close(fd);

        /*get the size of allocated memory in kernel*/
        fd = open("/proc/memshare/phymem_size", O_RDONLY);
        if(fd < 0)
        {
                printf("cannot open file /proc/memshare/phymem_size\n");
                return 0;
        }
        read(fd, s, sizeof(s));
        sscanf(s, "%lu", &phymem_size);
        close(fd);
        
        printf("phymem_addr=%lx, phymem_size=%lu\n", phymem_addr, phymem_size);
        /*memory map*/
        int map_fd = open("/dev/mem", O_RDWR);
        if(map_fd < 0)
        {
                printf("cannot open file test.txt  /dev/mem\n");
                return 0;
        }
        //map_addr = mmap(NULL, phymem_size, PROT_READ|PROT_WRITE, MAP_SHARED, map_fd, phymem_addr);
        //if(map_addr==MAP_FAILED)
        //  perror("mmap failed");
        //strcpy(map_addr, argv[1]);
        int addr;
        char buf[10];
        addr=lseek(map_fd,phymem_addr,SEEK_SET);
        if(addr<0)
          perror("lseek error");
        printf("0x%08lx\n",addr);
        
        read(map_fd,buf,10);
        printf("value=%s\n",buf);
        //munmap(map_addr, phymem_size);
        close(map_fd);
        return 0;
        
}
