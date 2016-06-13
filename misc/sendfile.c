#include <unistd.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
  int fd;
  off_t off = 0; 

  fd = open("file", O_RDWR | O_TRUNC | O_SYNC | O_CREAT, 0644); 
  ftruncate(fd, 2); 
  lseek(fd, 0, SEEK_END); 
  sendfile(fd, fd, &off, 0xfffffff); 
  return 0;
}
