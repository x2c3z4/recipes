#include <linux/init.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/mm_types.h>


void *addr;
struct scatterlist sg;
char result[128];
struct crypto_hash *tfm;
struct hash_desc desc;
struct task_struct *test_tsk,*tsk;

static void hexdump(unsigned char *buf, unsigned int len)
{
  print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
      16, 1,
      buf, len, false);
}
static int test_hash_jiffies_digest(struct hash_desc *desc,
            struct scatterlist *sg, int blen,
            char *out, int sec)
{
  unsigned long start, end;
  int bcount;
  int ret;

  for (start = jiffies, end = start + sec * HZ, bcount = 0;
       time_before(jiffies, end); bcount++) {
    ret = crypto_hash_digest(desc, sg, blen, out);
    if (ret)
      return ret;
  }

  //printk("%6u opers/sec, %9lu bytes/sec\n",
 //        bcount / sec, ((long)bcount * blen) / sec);
 printk("bcount=%d,sec=%d\n",bcount,sec);
  printk("hash time =%d ns/hash\n",sec*1000000000/bcount);
  return 0;
}
static int testd(void *arg)
{
  printk("enter testd thread...\n");
  test_hash_jiffies_digest(&desc,&sg,1,result,2);
  return 0;
}
static int __init pagehash_init(void){
  printk("~~~~~~~~~pagehash_init~~\n");
  struct vm_area_struct *tmp;
  addr = 0;
  tsk = current;
  tmp = tsk->mm->mmap;
  //tsk->mm->mm_count++;
  printk("%d(%s):mm_count=%d,mm_users=%d\n",tsk->pid,tsk->comm,tsk->mm->mm_count,tsk->mm->mm_users);

  while( (tmp != NULL) && (addr <= 0) ){
    addr = tmp->vm_start;
    tmp = tmp->vm_next;
  printk("addr=0x%x\n",(unsigned long)addr);
  }

  if(!addr)
    goto err_free;
  tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);
  if (IS_ERR(tfm))
    goto err_alloc;
  sg_init_one(&sg,addr,PAGE_SIZE);
  desc.tfm = tfm;
  desc.flags = 0;

  test_tsk = kthread_create(testd,NULL,"hash_test");
  wake_up_process(test_tsk);
  return 0;
  //test_hash_jiffies_digest(&desc,&sg,1,result,2);
  //hexdump(result,128);
err_alloc:
  crypto_free_hash(tfm);
err_free:
  return -1;
}
static void __exit pagehash_exit(void){
  crypto_free_hash(tfm);
  //tsk->count--;
  //printk("%d(%s):mm_count=%d,mm_users=%d\n",tsk->pid,tsk->comm,tsk->mm->mm_count,tsk->mm->mm_users);
  printk("~~~~~~~~~pagehash_exit~~\n");
}
module_init(pagehash_init);
module_exit(pagehash_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fengli <lifeng1519@gmail.com>");
