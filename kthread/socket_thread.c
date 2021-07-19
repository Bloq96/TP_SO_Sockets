#include <linux/fs.h>
#include <linux/in.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/socket.h>
//#include <linux/types.h>
#include <linux/uaccess.h>
//#include <linux/unistd.h>
//#include <netdb.h>

#define MAX_PENDING 5
#define MESSAGE_MAX_LENGTH 256
#define SERVER_PORT 54321

static struct task_struct *threadInfo;

static int socketServer(void *unused) {
    char buffer[MESSAGE_MAX_LENGTH];
    unsigned int addressLenght = sizeof(socketAddress);
    unsigned int messageLenght;
    struct socket *defaultSocket;
    int err;

    printk(KERN_INFO "Starting socket server...\n");
    printk(KERN_INFO "Kernel thread created as: %s [PID=%d]\n",
    current->comm,current->pid);

    err = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &defaultSocket);
    if (err < 0) {
        printk(KERN_INFO "Error: Could not create socket.\n");
        return -1;
    }
    if ((bind(defaultSocket, (struct sockaddr *)&defaultSocket,
    sizeof(defaultSocket))) < 0) {
        printk(KERN_INFO  "Error: Could not bind socket to address.\n");
        return -1;
    }
    listen(defaultSocket, MAX_PENDING);

    //while(1) {
        printk(KERN_INFO "Aguardando conexão do client.\n");
        if ((newConnectionSocket = accept(defaultSocket,
	(struct sockaddr *)&defaultSocket, &addressLenght)) < 0) {
            printk(KERN_INFO "Error: Could not connect to client.\n");
            return -1;
        }
        printk(KERN_INFO "Connection established.\n");
	messageLenght = 14;
        buffer[0] = 'H'; buffer[1] = 'e'; buffer[2] = 'l';
       	buffer[3] = 'l'; buffer[4] = 'o'; buffer[5] = ',';
	buffer[6] = ' '; buffer[7] = 'w'; buffer[8] = 'o';
	buffer[9] = 'r'; buffer[10] = 'l'; buffer[11] = 'd';
        buffer[12] = '!'; buffer[13] = '\n';	
        send(newConnectionSocket, buffer, messageLenght, 0);
        printk(KERN_INFO "Message sent.\n");
	while(recv(newConnectionSocket, buffer, sizeof(buffer),0));
        close(newConnectionSocket);
        printk(KERN_INFO "Client disconnected.\n");
        sock_release(defaultSocket);
        printk(KERN_INFO "Socket connection closed.\n");
	return 0;
    //}
}

static int printLine(void *unused) {
    struct file *outputFile;
    char string[13] = {'H','e','l','l','o',',','w','o','r','l','d',
    '!','\n'};
    loff_t pos = 0;
    printk(KERN_INFO "Dummy thread starting...\n");
    printk(KERN_INFO "Kernel thread created as: %s [PID=%d]",
    current->comm,current->pid);
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
