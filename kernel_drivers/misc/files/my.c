#include <linux/module.h>
#include "rwfile.h"

static struct file* fp;

#define FILE_NAME "/var/rwfile"
#define MAX_SIZE 100

static int __init my_init(void)
{
  int ret = 0;
  char *p;

  fp = file_open(FILE_NAME,O_CREAT | O_RDWR, S_IWUSR);
  if(!fp)
    return -1;
  p = "abcdefg";
  file_write(fp,0,p,MAX_SIZE);
  return ret;
}
static void __exit my_exit(void)
{
  file_close(fp);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL v2");

