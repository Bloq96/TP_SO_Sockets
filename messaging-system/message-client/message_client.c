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
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/uio.h>

#define MAX_PENDING 5
#define MESSAGE_MAX_LENGTH 256
#define SERVER_PORT 54321

static int initClientSocket(struct socket **newSocket) {
    int len = -1;

    len = sock_create_kern(&init_net,PF_INET,SOCK_STREAM,IPPROTO_TCP,
    newSocket);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not create socket.\n");
        return -1;
    }

    return 0;
}

static int connectToServer(struct socket *clientSocket,
struct sockaddr_in *serverAddress,unsigned int ipAddress,
unsigned short port) {
    int len = -1;

    memset(serverAddress,0,sizeof(*serverAddress));
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(SERVER_PORT);
    serverAddress->sin_addr.s_addr = htonl(ipAddress);

    len = clientSocket->ops->connect(clientSocket,
    (struct sockaddr *)serverAddress,sizeof(*serverAddress),O_RDWR);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not connect.\n");
        return -1;
    }

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
        printk(KERN_INFO "Client: Error: Could not send message\n");
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
        printk(KERN_INFO "Client: Error: Could not receive message.\n");
        return -1;
    }
    (*message)[len] = '\0';
    return len;
}

int __sys_message_client(char requestType, char *request, char *reply)
{
    unsigned int messageLength;
    char inputBuffer[MESSAGE_MAX_LENGTH+2],
    outputBuffer[MESSAGE_MAX_LENGTH+2];
    struct sockaddr_in serverAddress;
    struct socket *defaultSocket;
    int it = 0, len = -1;

    printk(KERN_INFO "Client: Starting socket client...\n");

    len = initClientSocket(&defaultSocket);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not create server socket.\n");
        return -1;
    }

    printk(KERN_INFO "Client: Connecting to server in 127.0.0.1.\n");

    len = connectToServer(defaultSocket,&serverAddress,0x7F000001,
    SERVER_PORT);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not connect to server.\n");
        sock_release(defaultSocket);
        return -1;
    }

    outputBuffer[0] = requestType;
    messageLength = 1;

    if(requestType == 'W') {
        for(it=1;it<MESSAGE_MAX_LENGTH+1;++it) {
            if(request[it-1]=='\0') {
	        break;
	    }
	    outputBuffer[it] = request[it-1];
	}
	messageLength = it;
    }

    outputBuffer[messageLength] = '\0';

    printk(KERN_INFO "Client: Connection established.\n");
    
    len = sendMessage(defaultSocket,&outputBuffer,
    messageLength);
    if (len < 0) {
        printk(KERN_INFO "CLient: Error: Could not send message.\n");
        return -1;
    }

    printk(KERN_INFO "Client: Making request.\n");
    printk(KERN_INFO "Client: Message is %s\n", outputBuffer);

    len = receiveMessage(defaultSocket,&inputBuffer);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not receive message.\n");
        return -1;
    }

    for(it=0;it<MESSAGE_MAX_LENGTH+1;++it) {
        reply[it] = inputBuffer[it+1];
	if(reply[it]=='\0') {
	    break;
	}
    }

    printk (KERN_INFO "Client: Server replies: %c: %s [%d]\n",
    inputBuffer[0],reply,len);

    sock_release(defaultSocket);
    printk(KERN_INFO "Client: Socket connection closed.\n");

    return 0;
}

SYSCALL_DEFINE3(message_client, char, requestType, char *, request,
char *, reply) {
    return __sys_message_client(requestType, request, reply);
}
