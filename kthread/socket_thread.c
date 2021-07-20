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
#include <linux/uio.h>
//#include <linux/unistd.h>
//#include <netdb.h>

#define MAX_PENDING 5
#define MESSAGE_MAX_LENGTH 256
#define SOCKET_PORT 54321

static struct task_struct *threadInfo;

static int socketServer(void *unused) {
    char outputBuffer[MESSAGE_MAX_LENGTH],
    inputBuffer[MESSAGE_MAX_LENGTH];
    unsigned int messageLenght;
    struct socket *defaultSocket, *newConnectionSocket;
    struct sockaddr_in socketAddress;
    int err = -1;
    struct msghdr outputMessage, inputMessage;
    struct iovec iovW[1], iovR[1];

    printk(KERN_INFO "Starting socket server...\n");
    printk(KERN_INFO "Kernel thread created as: %s [PID=%d]\n",
    current->comm,current->pid);

    err = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP,
    &defaultSocket);
    if (err < 0) {
        printk(KERN_INFO "Error: Could not create socket.\n");
        return -1;
    }
   
    memset(&socketAddress,0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(SOCKET_PORT);
    socketAddress.sin_addr.s_addr = INADDR_ANY;

    err = defaultSocket->ops->bind(defaultSocket,
    (struct sockaddr *)&socketAddress,sizeof (socketAddress));
    if (err < 0) {
        printk(KERN_INFO "Error: Could not bind socket to address.\n");
        return -1;
    }
    err = defaultSocket->ops->listen(defaultSocket, MAX_PENDING);
    if (err < 0) {
        printk(KERN_INFO "Error: Could not listen to socket.\n");
        return -1;
    }

    printk(KERN_INFO "Aguardando conexão do client.\n");

    newConnectionSocket = sock_alloc();
    newConnectionSocket->type = defaultSocket->type;
    newConnectionSocket->ops = defaultSocket->ops;
    err = defaultSocket->ops->accept(defaultSocket,
    newConnectionSocket, 0, false);
    if (err < 0) {
        printk(KERN_INFO "Error: Could not connect.\n");
        return -1;
    }

    printk(KERN_INFO "Connection established.\n");
  
    messageLenght = 14;
    outputBuffer[0] = 'H'; outputBuffer[1] = 'e';
    outputBuffer[2] = 'l'; outputBuffer[3] = 'l';
    outputBuffer[4] = 'o'; outputBuffer[5] = ',';
    outputBuffer[6] = ' '; outputBuffer[7] = 'w';
    outputBuffer[8] = 'o'; outputBuffer[9] = 'r';
    outputBuffer[10] = 'l'; outputBuffer[11] = 'd';
    outputBuffer[12] = '!'; outputBuffer[13] = '\n';

    outputMessage.msg_name = NULL;
    outputMessage.msg_namelen = 0;
    outputMessage.msg_control = NULL;
    outputMessage.msg_controllen = 0;
    outputMessage.msg_flags = MSG_DONTWAIT;
    outputMessage.msg_iocb = NULL;
    iovW[0].iov_len = messageLenght;
    iovW[0].iov_base = (char *) outputBuffer;
    iov_iter_init(&outputMessage.msg_iter, WRITE, iovW, 1,
    messageLenght);
    
    sock_sendmsg(newConnectionSocket, &outputMessage);
    printk(KERN_INFO "Message sent.\n");

    inputMessage.msg_name = NULL;
    inputMessage.msg_namelen = 0;
    inputMessage.msg_control = NULL;
    inputMessage.msg_controllen = 0;
    inputMessage.msg_flags = MSG_DONTWAIT;
    inputMessage.msg_iocb = NULL;
    iovR[0].iov_len = MESSAGE_MAX_LENGTH;
    iovR[0].iov_base = (char *) inputBuffer;
    iov_iter_init(&inputMessage.msg_iter, READ, iovR, 1,
    MESSAGE_MAX_LENGTH);

    while(sock_recvmsg(newConnectionSocket, &inputMessage, 0));
    printk(KERN_INFO "Message sent.\n");
    sock_release(newConnectionSocket);
    printk(KERN_INFO "Client disconnected.\n");
    sock_release(defaultSocket);
    printk(KERN_INFO "Socket connection closed.\n");
    return 0;
}

static int __init initThread(void) {
    printk(KERN_INFO "Dummy thread creation\n");
    threadInfo = kthread_create(socketServer,NULL,"Dummy Thread 1");
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
