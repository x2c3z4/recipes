#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/seq_file.h>
#include <linux/debugfs.h>

static struct dentry *binder_debugfs_dir_entry_root;
static struct dentry *binder_debugfs_dir_entry_proc;
static struct dentry *binder_debugfs_files[5];
static struct workqueue_struct *binder_deferred_workqueue;


void binder_deferred_func(struct work_struct *work)
{
  printk("doing something\n");
  
  printk("done\n");
}

static DECLARE_DELAYED_WORK(binder_deferred_work, binder_deferred_func);
/*
 *binder device fops
 * */
static unsigned int binder_poll(struct file *filp,
            struct poll_table_struct *wait)
{
  return 0;
}

static int binder_open(struct inode *nodp, struct file *filp)
{
  return 0;
}
static long binder_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
  return 0;
}
  
static int binder_mmap(struct file *filp, struct vm_area_struct *vma)
{
  return 0;
}

static int binder_flush(struct file *filp, fl_owner_t id)
{
  return 0;
}

static int binder_release(struct inode *nodp, struct file *filp)
{
  return 0;
}
/* 
 *proc debugfs
 * */

static int binder_state_show(struct seq_file *m, void *unused)
{
  return 0;
}

static int binder_stats_show(struct seq_file *m, void *unused)
{
  return 0;
}


static int binder_transactions_show(struct seq_file *m, void *unused)
{
  return 0;
}


static int binder_transaction_log_show(struct seq_file *m, void *unused)
{
  return 0;
}

#define binder_DEBUG_ENTRY(name) \
static int binder_##name##_open(struct inode *inode, struct file *file) \
{ \
  return single_open(file, binder_##name##_show, inode->i_private); \
} \
\
static const struct file_operations binder_##name##_fops = { \
  .owner = THIS_MODULE, \
  .open = binder_##name##_open, \
  .read = seq_read, \
  .llseek = seq_lseek, \
  .release = single_release, \
}

//binder_DEBUG_ENTRY(proc);
static const struct file_operations binder_fops = {
  .owner = THIS_MODULE,
  .poll = binder_poll,
  .unlocked_ioctl = binder_ioctl,
  .mmap = binder_mmap,
  .open = binder_open,
  .flush = binder_flush,
  .release = binder_release,
};

static struct miscdevice binder_miscdev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "binder",
  .fops = &binder_fops
};

binder_DEBUG_ENTRY(state);
binder_DEBUG_ENTRY(stats);
binder_DEBUG_ENTRY(transactions);
binder_DEBUG_ENTRY(transaction_log);

/*
*binder设备在系统启动时初始化
*/

static int __init binder_init(void)
{
  int ret=0;
  static unsigned long onesec;

  ret = misc_register(&binder_miscdev);//注册binder为misc的字符设备
//创建一个延时工作队列
  binder_deferred_workqueue = create_singlethread_workqueue("binder");
  if (!binder_deferred_workqueue)
    return -ENOMEM;
//以下创建层级debug proc目录,方便调试
//创建一下目录/proc/binder/proc/和文件/proc/binder/[state|stats|transactions|transaction_log|failed_transaction_log]
  binder_debugfs_dir_entry_root = debugfs_create_dir("binder", NULL);
  if (binder_debugfs_dir_entry_root){
    binder_debugfs_dir_entry_proc = debugfs_create_dir("proc",
             binder_debugfs_dir_entry_root);
  }
  if (binder_debugfs_dir_entry_root) {
    binder_debugfs_files[0] = debugfs_create_file("state",
            S_IRUGO,
            binder_debugfs_dir_entry_root,
            NULL,
            &binder_state_fops);
    binder_debugfs_files[1] =debugfs_create_file("stats",
            S_IRUGO,
            binder_debugfs_dir_entry_root,
            NULL,
            &binder_stats_fops);
    binder_debugfs_files[2] =debugfs_create_file("transactions",
            S_IRUGO,
            binder_debugfs_dir_entry_root,
            NULL,
            &binder_transactions_fops);
    binder_debugfs_files[3] =debugfs_create_file("transaction_log",
            S_IRUGO,
            binder_debugfs_dir_entry_root,
            NULL,
            &binder_transaction_log_fops);
    binder_debugfs_files[4] =debugfs_create_file("failed_transaction_log",
            S_IRUGO,
            binder_debugfs_dir_entry_root,
            NULL,
            &binder_transaction_log_fops);
  }
  onesec = msecs_to_jiffies(20000);
  queue_delayed_work(binder_deferred_workqueue,&binder_deferred_work,onesec);
  printk("init binder\n");
  return ret;
}

static void __exit binder_exit(void)
{
  int i;
  printk("exiting binder\n");
  
  if (binder_debugfs_dir_entry_root) {
    for( i = 0; i < 5; i++){
      debugfs_remove(binder_debugfs_files[i]);
    }
  }
  
  if(binder_debugfs_dir_entry_proc){
    debugfs_remove(binder_debugfs_dir_entry_proc);
  }
    
  if(binder_debugfs_dir_entry_root){
    debugfs_remove(binder_debugfs_dir_entry_root);
  }
  
  if ( !cancel_delayed_work(&binder_deferred_work) ){
    printk("flush workqueue\n");
    flush_workqueue(binder_deferred_workqueue);
  }
        
  destroy_workqueue(binder_deferred_workqueue);

  misc_deregister(&binder_miscdev);
  
  printk("exit binder\n");
}
module_init(binder_init);
module_exit(binder_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("fengli <lifeng1519@gmail.com>");

