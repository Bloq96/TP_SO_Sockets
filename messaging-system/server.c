#include <linux/fs.h>
#include <linux/in.h>
#include <linux/init.h>
#include <linux/inet.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/seq_file_net.h>
#include <linux/socket.h>
#include <linux/uaccess.h>
#include <linux/uio.h>

#define MAX_PENDING 5
#define MESSAGE_MAX_LENGTH 256
#define MAX_MESSAGES 2
#define SERVER_PORT 54321
#define MAX_CONNECTIONS 10000

static struct task_struct *threadInfo;

static int initServerSocket(struct socket **newSocket,
struct sockaddr_in *serverAddress,unsigned int ipAddress,
unsigned short port) {
    int len = -1;

    len = sock_create_kern(&init_net,PF_INET,SOCK_STREAM,IPPROTO_TCP,
    newSocket);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not create socket.\n");
        return -1;
    }

    memset(serverAddress,0, sizeof(*serverAddress));
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(port);
    serverAddress->sin_addr.s_addr = htonl(ipAddress);

    len = (*newSocket)->ops->bind(*newSocket,
    (struct sockaddr *)serverAddress,sizeof(*serverAddress));
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not bind socket to address.\n");
        return -1;
    }
    len = (*newSocket)->ops->listen(*newSocket,MAX_PENDING);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not listen to socket.\n");
        return -1;
    }

    return 0;
}

static int acceptConnection(struct socket *serverSocket,
struct socket **newConnectionSocket, struct sockaddr_in *socketAddress) {
    int len = -1;
    (*newConnectionSocket)= sock_alloc();
    (*newConnectionSocket)->type = serverSocket->type;
    (*newConnectionSocket)->ops = serverSocket->ops;
    len = serverSocket->ops->accept(serverSocket,
    *newConnectionSocket, 0, false);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not connect.\n");
        return -1;
    }
    (*newConnectionSocket)->ops->getname(*newConnectionSocket,
    (struct sockaddr *)socketAddress,2);
    return 0;
}

static void configMessage(struct msghdr *message,struct kvec (*kv)[1],
struct iovec (*iov)[1],char (*messageBuffer)[MESSAGE_MAX_LENGTH+2],
unsigned long int messageLength,unsigned int mode) {
    message->msg_name = NULL;
    message->msg_namelen = 0;
    message->msg_control = NULL;
    message->msg_controllen = 0;
    message->msg_flags = MSG_DONTWAIT;
    message->msg_iocb = NULL;
    (*iov)[0].iov_len = messageLength;
    (*iov)[0].iov_base = *messageBuffer;
    iov_iter_init(&message->msg_iter,mode,*iov,1,1);
    (*kv)[0].iov_len = messageLength;
    (*kv)[0].iov_base = *messageBuffer;
}

static int sendMessage(struct socket *socketConnection,
char (*message)[MESSAGE_MAX_LENGTH+2],unsigned long int messageLength) {
    struct msghdr outputMessage;
    struct iovec iov[1];
    struct kvec kv[1];
    int len = -1;

    configMessage(&outputMessage,&kv,&iov,message,messageLength,WRITE
    );

    len = kernel_sendmsg(socketConnection,&outputMessage,kv,
    1,kv[0].iov_len);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not send message\n");
        return -1;
    }
    return len;
}

static int receiveMessage(struct socket *socketConnection,
char (*message)[MESSAGE_MAX_LENGTH+2]) {
    struct msghdr inputMessage;
    struct iovec iov[1];
    struct kvec kv[1];
    int len = -1;

    configMessage(&inputMessage,&kv,&iov,message,MESSAGE_MAX_LENGTH+2,
    READ);

    len = kernel_recvmsg(socketConnection,&inputMessage,kv,
    1,kv[0].iov_len,0);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not receive message.\n");
        return -1;
    }
    (*message)[len] = '\0';
    return len;
}

static int socketServer(void *unused) {
    unsigned int messageLength;
    char outputBuffer[MESSAGE_MAX_LENGTH+2],
    inputBuffer[MESSAGE_MAX_LENGTH+2],
    buffer[MAX_MESSAGES+1][MESSAGE_MAX_LENGTH+2];
    struct sockaddr_in serverAddress, clientAddress;
    struct socket *defaultSocket, *newConnectionSocket;
    int it = 0, len = -1;
    int bufferStart = 0, bufferEnd = 0,
    messagesLengths[MAX_MESSAGES+1];
    int connectionCount = 0;

    printk(KERN_INFO "Server: Starting socket server...\n");
    printk(KERN_INFO "Server: Kernel thread created as: %s [PID=%d]\n",
    current->comm,current->pid);

    len = initServerSocket(&defaultSocket,&serverAddress,0x7F000001,
    SERVER_PORT);
    if (len < 0) {
        printk(KERN_INFO "Server: Error: Could not create server socket.\n");
        return -1;
    }

    while(connectionCount<MAX_CONNECTIONS) {
        printk(KERN_INFO "Server: Waiting for client connections.\n");
    
        len = acceptConnection(defaultSocket,&newConnectionSocket,
        &clientAddress);
        if (len < 0) {
            printk(KERN_INFO "Server: Error: Could not connect.\n");
            return -1;
        }

        printk(KERN_INFO "Server: Connection established.\n");

	++connectionCount;

        len = receiveMessage(newConnectionSocket,&inputBuffer);
        if (len < 0) {
            printk(KERN_INFO "Sever: Error: Could not receive message.\n"
	    );
            return -1;
        }
	switch(inputBuffer[0]) {
	    case 'R':
		if(bufferStart==bufferEnd) {
		    outputBuffer[0] = 'E';
		    outputBuffer[1] = 'E';
		    outputBuffer[2] = 'm';
		    outputBuffer[3] = 'p';
		    outputBuffer[4] = 't';
		    outputBuffer[5] = 'y';
		    outputBuffer[6] = ' ';
		    outputBuffer[7] = 'b';
		    outputBuffer[8] = 'u';
		    outputBuffer[9] = 'f';
		    outputBuffer[10] = 'f';
		    outputBuffer[11] = 'e';
		    outputBuffer[12] = 'r';
		    outputBuffer[13] = '!';
		    messageLength = 14;
		} else {
		    outputBuffer[0] = 'M';
                    for(it=1;it<MESSAGE_MAX_LENGTH+2;++it) {
		        outputBuffer[it] = buffer[bufferStart][it];
			if(outputBuffer[it]=='\0') {
			    break;
			}
		    }
		    messageLength = messagesLengths[bufferStart];
                    printk(KERN_INFO "Server: Sending %s [%d - %d - %d]\n",
		    outputBuffer,bufferStart,bufferEnd,messageLength);
		    bufferStart = 
		    (bufferStart+1)%(MAX_MESSAGES+1);
		}
		break;
	    case 'W':
		if(bufferStart==(bufferEnd+1)%(MAX_MESSAGES+1)) {
		    outputBuffer[0] = 'E';
		    outputBuffer[1] = 'B';
		    outputBuffer[2] = 'u';
		    outputBuffer[3] = 'f';
		    outputBuffer[4] = 'f';
		    outputBuffer[5] = 'e';
		    outputBuffer[6] = 'r';
		    outputBuffer[7] = ' ';
		    outputBuffer[8] = 'f';
		    outputBuffer[9] = 'u';
		    outputBuffer[10] = 'l';
		    outputBuffer[11] = 'l';
		    outputBuffer[12] = '!';
		    messageLength = 13;
		} else {
		    outputBuffer[0] = 'A';
		    outputBuffer[1] = 'M';
		    outputBuffer[2] = 'e';
		    outputBuffer[3] = 's';
		    outputBuffer[4] = 's';
		    outputBuffer[5] = 'a';
		    outputBuffer[6] = 'g';
		    outputBuffer[7] = 'e';
		    outputBuffer[8] = ' ';
		    outputBuffer[9] = 'a';
		    outputBuffer[10] = 'c';
		    outputBuffer[11] = 'c';
		    outputBuffer[12] = 'e';
		    outputBuffer[13] = 'p';
		    outputBuffer[14] = 't';
		    outputBuffer[15] = 'e';
		    outputBuffer[16] = 'd';
		    outputBuffer[17] = '!';
		    messageLength = 18;
                    for(it=0;it<MESSAGE_MAX_LENGTH+2;++it) {
		        buffer[bufferEnd][it] = inputBuffer[it];
			if(buffer[bufferEnd][it]=='\0') {
			    break;
			}
		    }
		    messagesLengths[bufferEnd] = len;
                    printk(KERN_INFO "Server: Client sent %s [%d - %d - %d]\n",
		    buffer[bufferEnd],bufferStart,bufferEnd,len);
		    bufferEnd = (bufferEnd+1)%(MAX_MESSAGES+1);
		}
                break;
	    default:
		outputBuffer[0] = 'E';
		outputBuffer[1] = 'U';
		outputBuffer[2] = 'n';
		outputBuffer[3] = 'k';
		outputBuffer[4] = 'n';
		outputBuffer[5] = 'o';
		outputBuffer[6] = 'w';
		outputBuffer[7] = 'n';
		outputBuffer[8] = ' ';
		outputBuffer[9] = 'R';
		outputBuffer[10] = 'e';
		outputBuffer[11] = 'q';
		outputBuffer[12] = 'u';
		outputBuffer[13] = 'e';
		outputBuffer[14] = 's';
		outputBuffer[15] = 't';
		outputBuffer[16] = '!';
		messageLength = 17;
        }
  
        len = sendMessage(newConnectionSocket,&outputBuffer,
        messageLength);
        if (len < 0) {
            printk(KERN_INFO "Server: Error: Could not send message.\n"
	    );
            return -1;
        }

        printk(KERN_INFO "Server: Answering client.\n");
    
        while(receiveMessage(newConnectionSocket,&inputBuffer));
        sock_release(newConnectionSocket);
        printk(KERN_INFO "Server: Client disconnected.\n");
    }

    sock_release(defaultSocket);
    printk(KERN_INFO "Server: Socket connection closed.\n");

    return 0;
}

static int __init initThread(void) {
    printk(KERN_INFO "Server: Dummy thread creation\n");
    threadInfo = kthread_create(socketServer,NULL,"Dummy Thread 1");
    if(IS_ERR(threadInfo)) {
        printk(KERN_INFO "Server: Failed creating the thread.");
    } else {
        printk(KERN_INFO "Server: Successfully created the thread.");
	wake_up_process(threadInfo);
    }
    return 0;
}

static void __exit threadCleanup(void) {
    printk(KERN_INFO "Server: Thread clean up.");
}

module_init(initThread);
module_exit(threadCleanup);
