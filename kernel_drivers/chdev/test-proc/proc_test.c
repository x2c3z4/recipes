#include<linux/init.h>
#include<linux/module.h>
#include<asm/uaccess.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/list.h>
#include<linux/types.h>

#include<linux/version.h>

#define BUFFERSIZE 1024
static int process_pid;

static struct proc_dir_entry *process_family_dir, *pid_file, *family_file;
struct task_struct *pid_to_task(pid_t pid)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
    return find_task_by_pid(pid);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)
    return find_task_by_vpid(pid);
#else
    return pid_task(find_vpid(pid), PIDTYPE_PID);
#endif
}

//proc file family related
static int proc_read_family(char *page,char **start,off_t off,int count,int *eof,void *data)
{
    printk("proc_read_family -----*start=0x%lx,**start=0x%lx,page=0x%lx,off=%u,count=%d,eof=%d\n",*start,&(*start),page,(unsigned int)off,count,*eof);
	char * offset=page;
	int retval=0;
	int len;
		
	struct task_struct *pid_struct, *pid_kin;
	struct list_head *kin_list;

	if(off)
		return 0;

	pid_struct=pid_to_task(process_pid);

	len=sprintf(offset,"The kins of process %s(pid=%d):\n\tRelationship\t pid\t gpid\t name\n",pid_struct->comm,pid_struct->pid);
	offset+=len;
	retval+=len;

	//pid_struct's parent
	pid_kin=pid_struct->parent;
	len=sprintf(offset,"\tparent     \t %d\t %d\t %s\n",pid_kin->pid,pid_kin->tgid,pid_kin->comm);
	offset+=len;
	retval+=len;

	//pid_struct's sibling are children of pid_struct's parent;
	list_for_each(kin_list,&pid_struct->parent->children){
                pid_kin=list_entry(kin_list,struct task_struct,sibling);
                len=sprintf(offset,"\tsibling     \t %d\t %d\t %s\n",pid_kin->pid,pid_kin->tgid,pid_kin->comm);
                offset+=len;
                retval+=len;
        }

	//pid_struct's children
	list_for_each(kin_list,&pid_struct->children){
		pid_kin=list_entry(kin_list,struct task_struct,sibling);
		len=sprintf(offset,"\tchildren    \t %d\t %d\t %s\n",pid_kin->pid,pid_kin->tgid,pid_kin->comm);
		offset+=len;
		retval+=len;
	}
    printk("retval=%d\n",retval);

    return retval;
}

//proc file pid related
static int proc_read_pid(char *page,char **start,off_t off,int count,int *eof,void *data)
{
    printk("proc_read_pid -----*start=0x%lx,**start=0x%lx,page=0x%lx,off=%u,count=%d,eof=%d\n",*start,&(*start),page,(unsigned int)off,count,*eof);
	int len;
	len=sprintf(page,"process pid : %d\n",process_pid);
    	printk("len=%d\n",len);
	return len;
}

static int proc_write_pid(struct file *file,const char * buffer,unsigned long count ,void * data)
{
	printk(KERN_ALERT "write again\n");
	char buf[BUFFERSIZE];
	int len, i;
	int tmp_pid_val=0;
	if(count>BUFFERSIZE-1)
		len=BUFFERSIZE-1;
	else
		len=count;

	if(copy_from_user(buf,buffer,len))
		return -EFAULT;
	buf[len]='\0';
	printk(KERN_ALERT  "proc_test: %s was entered, len=%d\n",buf,len);
	for(i=0;i<len;i++){
		printk(KERN_ALERT "proc_test: buf[%d]=%c(c)| %d(int) \n",i,buf[i],(int)buf[i]);
		if(buf[i]< '0' || buf[i]>'9')
			continue;
		tmp_pid_val=((int)buf[i]-'0') + tmp_pid_val*10;
		printk(KERN_ALERT "proc_test: tmp_pid_val=%d\n",tmp_pid_val);
	}
	printk(KERN_ALERT "proc_test: exit for \n");
	if(!pid_to_task(tmp_pid_val)){
		printk(KERN_ALERT "proc_test:Can't find process by pid :%d\n",tmp_pid_val);
		return -EFAULT;
	}
	else{
		process_pid=tmp_pid_val;
	}

	return len;
}

static int proc_init(void)
{
	//create proc dir process_family
	process_family_dir=proc_mkdir("process_family",NULL);
    /* 3.0 kernel don't have the owner ... */
	//process_family_dir->owner=THIS_MODULE;


	//create read-only proc file family
	family_file=create_proc_read_entry("family",0444,process_family_dir,proc_read_family,NULL);
	//family_file->owner=THIS_MODULE;

	//create read&write proc file pid
	pid_file=create_proc_entry("pid",0666,process_family_dir);
	//pid_file->owner=THIS_MODULE;
	pid_file->read_proc=proc_read_pid;
	pid_file->write_proc=proc_write_pid;
	process_pid=1;

	return 0;

}
static void proc_exit(void)
{
	remove_proc_entry("pid",process_family_dir);
	remove_proc_entry("family",process_family_dir);
	remove_proc_entry("process_family",NULL);
}
module_init(proc_init);
module_exit(proc_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("lfeng <lifeng1519@gmail.com>");
