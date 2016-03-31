#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>

#include "pagehash.h"


/* 主从设备号 */
static int pagehash_major=0;
static int pagehash_minor=0;


/* 设备类型和设备变量 */
static struct class *pagehash_class=NULL;
static struct pagehash_android_dev *hello_dev=NULL;

/* dev dir */
static int pagehash_open(struct inode *inode,struct file *filp);
static int pagehash_release(struct inode *inode,struct file *filp);
static ssize_t pagehash_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos);
static ssize_t pagehash_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos);


static struct file_operations pagehash_fops={
  .owner=THIS_MODULE,
  .open=pagehash_open,
  .release=pagehash_release,
  .read=pagehash_read,
  .write=pagehash_write,
};

/* 访问设置属性方法 */
static ssize_t pagehash_val_show(struct device *dev,struct device_attribute *attr,char *buf);
static ssize_t pagehash_val_store(struct device *dev,struct device_attribute *attr,const char *buf,size_t count);

/* 定义设备属性 */
static DEVICE_ATTR(val,S_IRUGO|S_IWUSR,pagehash_val_show,hello_val_store);
static int pagehash_open(struct inode *inode,struct file *filp)
{
  struct pagehash_android_dev *dev;

  dev=container_of(inode->i_cdev,struct pagehash_android_dev,dev);
  filp->private_data=dev;

  return 0;
}

static int pagehash_release(struct inode *inode,struct file *filp)
{
  return 0;
}



static ssize_t pagehash_read(struct file *filp,char __user *buf,size_t count,loff_t *f_pos)
{
  ssize_t err=0;
  struct pagehash_android_dev *dev=filp->private_data;

  if(down_interruptible(&(dev->sem))){
    return -ERESTARTSYS;
  }

  if( count < sizeof(dev->val)){
    goto out;
  }

  if(copy_to_user(buf,&(dev->val),sizeof(dev->val))){
    err=-EFAULT;
    goto out;
  }

  err=sizeof(dev->val);

out:
  up(&(dev->sem));
  return err;
}

static ssize_t pagehash_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
  ssize_t err=0;
  struct pagehash_android_dev *dev=filp->private_data;

  if(down_interruptible(&(dev->sem))){
    return -ERESTARTSYS;
  }

  if(count != sizeof(dev->val)){
    goto out;
  }

  if(copy_from_user(&(dev->val),buf,count)){
    err=-EFAULT;
    goto out;
  }

  err=sizeof(dev->val);

out:
  up(&(dev->sem));
  return err;
}

static ssize_t __pagehash_get_val(struct hello_android_dev *dev,char *buf)
{
  int val=0;

  if(down_interruptible(&(dev->sem))){
      return -ERESTARTSYS;
  }

  val=dev->val;
  up(&(dev->sem));

  return snprintf(buf,PAGE_SIZE,"%d\n",val);
}

static ssize_t __pagehash_set_val(struct hello_android_dev *dev,const char *buf,size_t count)
{
  int val=0;

  val=simple_strtol(buf,NULL,0);

  if(down_interruptible(&(dev->sem))){
    return -ERESTARTSYS;
  }

  dev->val=val;
  up(&(dev->sem));

  return count;
}
static ssize_t pagehash_val_show(struct device *dev,struct device_attribute *attr,char *buf)
{
  struct pagehash_android_dev *hdev=(struct hello_android_dev*)dev_get_drvdata(dev);
  return __pagehash_get_val(hdev,buf);
}

static ssize_t pagehash_val_store(struct device *dev,struct device_attribute *attr, const char *buf,size_t count)
{
  struct pagehash_android_dev *hdev=(struct hello_android_dev *)dev_get_drvdata(dev);

  return __pagehash_set_val(hdev,buf,count);
}

static ssize_t pagehash_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data)
{
  if(off>0){
    *eof=1;
    return 0;
  }

  return __pagehash_get_val(hello_dev,page);
}
static int pagehash_proc_write(struct file *file, const char __user *buffer,
            unsigned long count, void *data)
{
  int err=0;
  char *page=NULL;

  if(count > PAGE_SIZE){
    printk(KERN_ALERT"The buffer is too large:%lu\n",count);
    return -EFAULT;
  }

  page=(char*)__get_free_page(GFP_KERNEL);
  if(!page){
    printk(KERN_ALERT"Failed to alloc page\n");
    return -ENOMEM;
  }

  if(copy_from_user(page,buffer,count)){
    printk(KERN_ALERT"failed to copy buffer from user\n");
    err=-EFAULT;
    goto out;
  }

  err=__pagehash_set_val(hello_dev,page,count);
out:
  free_page((unsigned long)page);
  return err;
}

static void pagehash_create_proc(void)
{
  struct proc_dir_entry *entry;

  entry=create_proc_entry(pagehash_DEVICE_PROC_NAME,0,NULL);
  if(entry){
    //entry->owner=THIS_MODULE;
    entry->read_proc=pagehash_proc_read;
    entry->write_proc=pagehash_proc_write;
  }
}

static void pagehash_remove_proc(void)
{
  remove_proc_entry(pagehash_DEVICE_PROC_NAME,NULL);
}

static int __pagehash_setup_dev(struct hello_android_dev *dev)
{
  int err;

  dev_t devno=MKDEV(pagehash_major,hello_minor);

  memset(dev,0,sizeof(struct pagehash_android_dev));
  cdev_init(&(dev->dev),&pagehash_fops);
  dev->dev.owner=THIS_MODULE;

  err=cdev_add(&(dev->dev),devno,1);

  if(err){
    return err;
  }

  init_MUTEX(&(dev->sem));
  dev->val=0;

  return 0;
}
struct crypto_tfm *tfm;
char *do_digest(struct page *chr) {
  struct crypto_hash *tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);
if (tfm == NULL)
    fail;
char *output_buf = kmalloc(crypto_hash_digestsize(tfm), GFP_KERNEL);
if (output_buf == NULL)
    fail;
struct scatterlist sg[2];
struct hash_desc desc = {.tfm = tfm};
ret = crypto_hash_init(&desc);
if (ret != 0)
    fail;
sg_init_table(sg, ARRAY_SIZE(sg));
sg_set_buf(&sg[0], "Hello", 5);
sg_set_buf(&sg[1], " World", 6);
ret = crypto_hash_digest(&desc, sg, 11, output_buf);
if (ret != 0)
    fail;
} 

static int __init pagehash_init(void)
{
  int err=-1;
  printk("Initializing pagehash device\n");

  err=alloc_chrdev_region(&dev,0,1,pagehash_DEVICE_NODE_NAME);
  if(err){
    printk("failed alloc char device region\n");
    goto fail;
  }

  pagehash_major=MAJOR(dev);
  pagehash_minor=MINOR(dev);


  pagehash_dev=kmalloc(sizeof(struct hello_android_dev),GFP_KERNEL);
  if(!pagehash_dev){
    err=-ENOMEM;
    printk("failed to malloc pagehash_android_dev \n");
    goto unregister;
  }

  err=__pagehash_setup_dev(hello_dev);
  if(err){
    printk("failed to add char dev to system\n");
    goto cleanup;
  }


  /* /sys/class */
  pagehash_class=class_create(THIS_MODULE,HELLO_DEVICE_CLASS_NAME);
  if(IS_ERR(pagehash_class)){
    err=PTR_ERR(pagehash_class);
    printk(KERN_ALERT"Failed to create pagehash class\n");
    goto destroy_cdev;
  }


  tmp=device_create(pagehash_class,NULL,dev,"%s",HELLO_DEVICE_FILE_NAME);
  if(IS_ERR(tmp)){
    err=PTR_ERR(tmp);
    printk(KERN_ALERT"Failed to create pagehash device\n");
    goto destroy_class;
  }

  err=device_create_file(tmp,&dev_attr_val);
  if(err){
    printk(KERN_ALERT"failed to create attribute val.");
    goto destroy_device;
  }

  dev_set_drvdata(tmp,pagehash_dev);

  pagehash_create_proc();

  printk("Successed to initialize pagehash device \n");
  return 0;

destroy_device:
  device_destroy(pagehash_class,dev);
destroy_class:
  class_destroy(pagehash_class);
destroy_cdev:
  cdev_del(&(pagehash_dev->dev));
cleanup:
  kfree(pagehash_dev);
unregister:
  unregister_chrdev_region(dev,1);
fail:
  return err;
}

static void __exit pagehash_exit(void)
{
  dev_t devno=MKDEV(pagehash_major,hello_minor);
  printk("release pagehash device\n");

  pagehash_remove_proc();

  if(pagehash_class){
    device_destroy(pagehash_class,devno);
    class_destroy(pagehash_class);
  }

  if(pagehash_dev){
    cdev_del(&(pagehash_dev->dev));
    kfree(pagehash_dev);
  }
  unregister_chrdev_region(devno,1);
  printk("successed to release pagehash device\n");
}


module_init(pagehash_init);
module_exit(pagehash_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fengli <lifeng1519@gmail.com>");
