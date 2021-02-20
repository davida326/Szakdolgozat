#include <linux/module.h>     /* Needed by all modules */ 
#include <linux/kernel.h>     /* Needed for KERN_INFO */ 
#include <linux/init.h>       /* Needed for the macros */ 
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/sched/signal.h>

struct task_struct *p;

///< The author -- visible when you use modinfo 
MODULE_AUTHOR("Vass Dávid Attila"); 
  
///< The description -- see modinfo 
MODULE_DESCRIPTION("Process information getter"); 
MODULE_LICENSE("GPL");
static int __init processinfo_init(void) 
{ 
    printk("**********************************");
    for_each_process(p)
        if(!p->rt_priority)
            printk(KERN_INFO "command:%s, pid:%d, priority:%d ,weight:%lu ,vruntime:%llu",p->comm,p->pid,p->prio,p->se.load.weight,p->se.vruntime);
} 
  
static void __exit processinfo_end(void) 
{ 
    printk(KERN_INFO "Goodbye world\n"); 
} 
  
module_init(processinfo_init); 
module_exit(processinfo_end); 
