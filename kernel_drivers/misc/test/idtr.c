#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#define CALLOFF 100 //读取100字节
 
struct {
  unsigned short limit;
  unsigned int base;
} __attribute__ ((packed)) idtr;  //这个结构表示IDTR寄存器，这个寄存器中保存中断描述符表 的地址
 
 
struct {
  unsigned short off1;
  unsigned short sel;
  unsigned char none,flags;
  unsigned short off2;
} __attribute__ ((packed)) idt;  //中断描述符表中的内容：中断门描述符
 
 
unsigned int old_readkmem (int fd, void * buf,size_t off,unsigned int size) //用read方式读取kmem中一定长度内容
{
  if (lseek64(fd, (unsigned long long)off,SEEK_SET)!=off)
  {
    //perror(\"fd lseek\");
    return 0;
  }
 
  if (read(fd, buf,size)!=size)
  {
    //perror(\"fd read\");
    return 0;
  }
 
}
 
unsigned long readkmem (int fd, void * buf, size_t off, unsigned int size)//用mmap方式从kmem中读取一定长度内容
{
  size_t  moff, roff;
  size_t   sz = getpagesize();
 
  char * kmap;
 
  unsigned long ret_old = old_readkmem(fd, buf, off, size); //先用老方法读取,不行再用mmap
  if (ret_old != 0)
    return ret_old;
 
  moff = ((size_t)(off/sz)) * sz;    
  roff = off - moff;  
 
  kmap = mmap(0, size+sz, PROT_READ, MAP_PRIVATE, fd, moff);
 
  if (kmap == MAP_FAILED)
  {
      perror("readkmem: mmap");
      return 0;
 }
 
 memcpy (buf, &kmap[roff], size);
 
 if (munmap(kmap, size) != 0)
 {
   perror("readkmem: munmap");
   return 0;
 }
 
 return size;
}
 
int main (int argc, char **argv)
{
 unsigned sys_call_off;
 int kmem_fd;  // /dev/kmem文件描述符
 unsigned sct;
 char sc_asm[CALLOFF],*p;

 /* 获得IDTR寄存器的值 */
 asm ("sidt %0" : "=m" (idtr));
 printf("idtr base at 0x%X\n",(int)idtr.base);
 
 /* 打开kmem */
 kmem_fd = open ("/dev/kmem",O_RDONLY);
 if (kmem_fd<0)
 {
     perror("open");
     return 1;
 }
 
 /* 从IDT读出0x80向量 (syscall) */
 readkmem (kmem_fd, &idt,idtr.base+8*0x80,sizeof(idt)); //idtr.base+8*0x80 表示80中断描述符的偏移
 sys_call_off = (idt.off2 << 16) | idt.off1;    //idt.off2 表示地址的前16位，得到syscall地址
 printf("idt80: flags=%X sel=%X off=%X\n", (unsigned)idt.flags,(unsigned)idt.sel,sys_call_off);
 
 /* 寻找sys_call_table的地址 */
 readkmem (kmem_fd, sc_asm,sys_call_off,CALLOFF);  
 
 p = (char*)memmem (sc_asm,CALLOFF,"\xff\x14\x85",3); //只要找到邻近int $0x80入口点system_call的call sys_call_table(,eax,4)指令的机器指令就可以了,call something(,eax,4)指令的机器码是0xff 0x14 0x85，因此搜索这个字符串。
 sct = *(unsigned*)(p+3); //sys_call_table地址就在0xff 0x14 0x85之后
 
 if (p)
 {
   printf ("sys_call_table at 0x%x, call dispatch at 0x%x\n", sct, p);
 }
 
 close(kmem_fd);
 return 0;
}
