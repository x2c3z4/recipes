#include <unistd.h>
#include <time.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define N   10000
double min=0.0f,avag=0.0f,max=0.0f,bw=0.0f;
int child_pid;
int fd0[2],fd1[2];
char data[1000];
clock_t start,end;
int i;
int main()
{
  pipe(fd0);
  pipe(fd1);
  if( (child_pid = fork()) > 0){//parent 
   close(fd0[0]);
   close(fd1[1]);
   bzero(data,sizeof(data));

     start = clock(); 
   for(i=0;i<N;i++){
     write(fd0[1],data,sizeof(data));
     read(fd1[0],data,sizeof(data));
   }
     end = clock();
     
     printf("Bandwidth=%fMB/s,Avag time=%fs\n",(double)N*1000/(end-start)*CLOCKS_PER_SEC/1000000,(double)(end-start)/CLOCKS_PER_SEC/N);
     close(fd0[1]);
     close(fd1[0]);
  }else if( child_pid == 0){
    close(fd0[1]);
    close(fd1[0]);
   for(i=0;i<N;i++){
     read(fd0[0],data,sizeof(data));
     write(fd1[1],data,sizeof(data));
   }
   close(fd0[0]);
   close(fd1[1]);
   exit(0);
  }else{
    perror("fork error");
  }
  wait(child_pid);
  return 0;

}
