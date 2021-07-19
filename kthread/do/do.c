#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/types.h>
#include <linux/uaccess.h>

static struct task_struct *threadInfo;

static int printLine(void *unused) {
    struct file *outputFile;
    char string[13] = {'H','e','l','l','o',',','w','o','r','l','d',
    '!','\n'};
    loff_t pos = 0;
    printk(KERN_INFO "Dummy thread starting...\n");
    printk(KERN_INFO "Input string: %s",string);
    outputFile = filp_open("/home/debian/Documents/kthread/output.txt",
    O_WRONLY|O_CREAT,0666);
    if(IS_ERR(outputFile)) {
        printk(KERN_INFO "Error opening the file!");
	return -1;
    }
    kernel_write(outputFile,string,13,&pos);
    filp_close(outputFile,NULL);
    printk(KERN_INFO "Dummy thread stopping...\n");
    return 0;
}

static int __init initThread(void) {
    printk(KERN_INFO "Dummy thread creation\n");
    threadInfo = kthread_create(printLine,NULL,"Dummy Thread 1");
    if(IS_ERR(threadInfo)) {
        printk(KERN_INFO "Failed creating the thread.");
    } else {
        printk(KERN_INFO "Successfully created the thread.");
	wake_up_process(threadInfo);
    }
    return 0;
}

static void __exit threadCleanup(void) {
    printk(KERN_INFO "Thread clean up.");
}

module_init(initThread);
module_exit(threadCleanup);
