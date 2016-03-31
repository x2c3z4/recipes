#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

int main()
{
   int size,i=0;
   int **ptr = NULL;
   int pid[6];
   pid_t my_pid = (pid_t) 0;
   char cmdStr[40];
   memset(cmdStr, 0, 40);
   unsigned long adbd_pid = 0;

   ptr = (int **)malloc(2000000*sizeof(int *));//Pointer to hold 2M
Another pointer
   int free_levels[6] = {1536,2048,4096,5120,5632,6144}; //

   //Overwriting the default lowmemorykiller module parameters
   system("cat /sys/module/lowmemorykiller/parameters/minfree");
   system("echo 1536,2048,4096,5120,5632,6144 >/sys/module/
lowmemorykiller/parameters/minfree");
   system("cat /sys/module/lowmemorykiller/parameters/minfree");
   system("cat /proc/sys/vm/min_free_kbytes");

   my_pid = getpid();

   sprintf(cmdStr, "echo 13 > /proc/%d/oom_adj", my_pid);
   system(cmdStr);

   while(i++ < 240)  //240 MB Memory
   {
	    size = 256*1024*sizeof(int); //1MB
	    ptr[i] = (int *)malloc(size);

	    int *write=ptr[i];
	    if(ptr[i]!=NULL)
	    {
		size = size/sizeof(int);
		//printf("Success in malloc");
		while(size--)
			*write++=size;
			system("/usr/bin/free");
	    }
	    else
	    {
		    printf("malloc error %d ",ptr[i]);
	    }
    }
}
