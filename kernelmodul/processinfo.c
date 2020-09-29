#include <linux/module.h>     /* Needed by all modules */ 
#include <linux/kernel.h>     /* Needed for KERN_INFO */ 
#include <linux/init.h>       /* Needed for the macros */ 
#include <linux/sched/signal.h>
#include <linux/sched.h>
#include <linux/sched/rt.h>
struct task_struct *p;
struct list_head *list;

///< The author -- visible when you use modinfo 
MODULE_AUTHOR("Vass Dávid Attila"); 
  
///< The description -- see modinfo 
MODULE_DESCRIPTION("Process information getter"); 
MODULE_LICENSE("GPL");
static int __init processinfo_init(void) 
{ 
    for(p=&init_task;(p=next_task(p))!=&init_task;)
    {
        printk(KERN_INFO "/comm/and: %s pid: %d prio: %d rt_prio: %d weight: %lu vruntime: %llu pcount:%lu \nsum_exec_runtime:%llu prev_sum_exec_runtime:%llu exec_start:%llu\n",p->comm,p->pid,p->prio,rt_task(p),p->se.load.weight,p->se.vruntime,p->sched_info.pcount,p->se.sum_exec_runtime,p->se.prev_sum_exec_runtime,p->se.exec_start);
        printk(KERN_INFO "utime: %llu stime: %llu gtime: %llu\n",p->utime,p->stime,p->gtime);
        printk(KERN_INFO "------------------------------------------------------------------\n");
    }
    
    return 0; 
} 
  
static void __exit processinfo_end(void) 
{ 
    printk(KERN_INFO "Goodbye world\n"); 
} 
  
module_init(processinfo_init); 
module_exit(processinfo_end); 
