#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/init.h> 
#include <linux/proc_fs.h> 
#include <linux/jiffies.h> 
#include <asm/uaccess.h> 
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/sched.h> 
#include <linux/time.h>
#define MODULE_NAME "Myproc" 
#define MYDATA_LEN   8200 
 
//放用户空间传入的数据 
struct my_proc_data{ 
   char value[MYDATA_LEN]; 
}; 
char str[20];
struct my_proc_data mydata;   
struct task_struct *task;
//proc结构变量 
static struct proc_dir_entry *example_dir; 
static struct proc_dir_entry *date_file; 
static int   param; 
module_param(param, int , 0644); 
 
//读文件 myfile的读驱动函数 
static int proc_read(char *page, char **start,off_t off,int count,int *eof,void *data ){ 
    int len=0 ; 
    struct my_proc_data *mydatap = (struct my_proc_data *) data;
    if(param ==0){
    	strcpy(mydata.value,"The parents process：\n");
      sprintf(mydata.value,"%s %d %s\n",mydata.value,current->pid,current->comm);//进程ID，进程名称
      task = NULL;
   	task = current;
   	//从初始进程开始向上遍历
   	while(task->pid!=1){
   	   task=task->parent;
   	   sprintf(mydata.value,"%s %d %s \n",mydata.value,task->pid, task->comm);
   	}
   	task = &init_task;//arch/x86/kernel.c
      // printk("当前的进程是：%d %s \n",current->pid,current->comm);
      sprintf(mydata.value,"%s %s \n",mydata.value, "Current task_struct:");
      for_each_process(task){
   	   sprintf(mydata.value,"%s %d %s \n",mydata.value,task->pid,task->comm);
   	   printk("%d %s\n",task->pid, task->comm);
      }
    	len+=sprintf(page,"%s\n",mydata.value);
   }
   else{
      struct timespec t1;
      struct timeval t2;
     t1 = current_kernel_time();
     do_gettimeofday(&t2);
   	sprintf(str,"%s","nsec=:");
   	len+=sprintf(page,"%s%ld\n%ld\n",str,t1.tv_nsec,t2.tv_usec);
   }
    //len+=sprintf(page,"%s\n",mydatap->value); 
    return len;    
}    
//写文件 myfile 的写驱动函数 
static int proc_write(struct file *file,const char *buffer,unsigned long count,void *data){ 
    int len ; 
    struct my_proc_data *mydatap = (struct my_proc_data *) data; 
    if(count>MYDATA_LEN) 
       len = MYDATA_LEN; 
    else   
       len = count; 
    if(copy_from_user(mydatap->value,buffer,len)){ 
       return -EFAULT; 
    } 
    mydatap->value[len-1] = '\0'; 
    return len;    
}    
//装入模块 
int init_module(void) 
{ 
   //创建 proc/myfile目录 
   example_dir = (struct proc_dir_entry * )proc_mkdir("part5dir",0); 
   if(example_dir == 0){
   	  printk("mkdir fail\n"); 
      return -1; 
   } 
   //创建/proc/mydir/myfile 文件 
   date_file = (struct proc_dir_entry * )create_proc_entry("part5",0666,example_dir); 
   if(date_file == 0){ 
      remove_proc_entry("part5",0); 
      printk("mkfile fail\n"); 
      return -ENOMEM; 
   }      
    
   
   date_file->data=&mydata; 
   date_file->read_proc=&proc_read; 
   date_file->write_proc=&proc_write; 
   //date_file->owner=THIS_MODULE; 
   return 0; 
} 
//卸载模块 
void cleanup_module(void) 
{ 
   remove_proc_entry("part5",example_dir); 
   remove_proc_entry("part5dir",NULL); 
   printk("Goodbye.\n"); 
} 
MODULE_LICENSE("GPL"); 
MODULE_DESCRIPTION("exp5"); 
MODULE_AUTHOR("wtz"); 
