#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>



if(scull_major)
{
    dev=MKDEV(scull_major,scull_minor);
    result=register_chrdev_region(dev,scull_nr_devs,"scull");
}else{
    result=alloc_chrdev_region(&dev,scull_minor,scull_nr_devs,"scull");
    scull_major=MAJOR(dev);
}

if(result<0){
    printk(KERN_WARNING"scull: can't get major %d\n",scull_major);
}

struct file_operation scull_fops={
.owner=THIS_MODULE,
.llseek=scull_llseek,
.read=scull_read,
.write=scull_write,
.ioctl=scull_ioctl,
.open=scull_open,
.release=scull_release,
};

struct scull_dev{
    struct scull_qset *data;
    int quantum;
    int qset;
    unsigned long size;
    unsigned long access_key;
    struct semaphore sem;
    struct cdev cdev;
};

static void scull_setup_cdev(struct scull_dev *dev,int index)
{

    int err,devno=MKDEV(scull_major,scull_minor+index);
    cdev_init(&dev->cdev,&scull_fops);
    dev->cdev.owner=THIS_MODULE;
    dev->cdev.ops=&scull_fops;

    err=cdev_add(&dev->cdev,devno,1);

    if(err<0)
        printk(KERN_NOTICE"Error %d adding scull %d ",err,index);
}


void scull_exit_cdev()
{
    unregister_chrdev_region();
}
