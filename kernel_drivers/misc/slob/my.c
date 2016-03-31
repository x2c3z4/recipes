#include <linux/module.h>
#include "fast_slob.h"

static struct fast_slob *slob;

static int __init my_init(void)
{
  int ret = 0;
  //slob = fast_slob_create(128000, 16 * 1024, 4, 2);
  slob = fast_slob_create(65,30, 4, 1);
  if (!slob)
    return -ENOMEM;
  return ret;
}
static void __exit my_exit(void)
{
	if(slob)
  		fast_slob_destroy(slob);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL v2");

