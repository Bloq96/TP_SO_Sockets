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
struct iovec (*iov)[1],char (*messageBuffer)[MESSAGE_MAX_LENGTH],
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
char (*message)[MESSAGE_MAX_LENGTH],unsigned long int messageLength) {
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
char (*message)[MESSAGE_MAX_LENGTH]) {
    struct msghdr inputMessage;
    struct iovec iov[1];
    struct kvec kv[1];
    int len = -1;

    configMessage(&inputMessage,&kv,&iov,message,MESSAGE_MAX_LENGTH,
    READ);

    len = kernel_recvmsg(socketConnection,&inputMessage,kv,
    1,kv[0].iov_len,0);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not receive message.\n");
        return -1;
    }
    return len;
}

static int socketClient(void) {
    unsigned int messageLength;
    char outputBuffer[MESSAGE_MAX_LENGTH],
    inputBuffer[MESSAGE_MAX_LENGTH];
    struct sockaddr_in serverAddress;
    struct socket *defaultSocket;
    int len = -1;

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

    printk(KERN_INFO "Client: Connection established.\n");
  
    messageLength = 14;
    outputBuffer[0] = 'H'; outputBuffer[1] = 'e';
    outputBuffer[2] = 'l'; outputBuffer[3] = 'l';
    outputBuffer[4] = 'o'; outputBuffer[5] = ',';
    outputBuffer[6] = ' '; outputBuffer[7] = 's';
    outputBuffer[8] = 'e'; outputBuffer[9] = 'r';
    outputBuffer[10] = 'v'; outputBuffer[11] = 'e';
    outputBuffer[12] = 'r'; outputBuffer[13] = '!';

    printk(KERN_INFO "Client: Greeting server.\n");

    len = sendMessage(defaultSocket,&outputBuffer,
    messageLength);
    if (len < 0) {
        printk(KERN_INFO "CLient: Error: Could not send message.\n");
        return -1;
    }

    printk(KERN_INFO "Client: Message sent.\n");

    len = receiveMessage(defaultSocket,&inputBuffer);
    if (len < 0) {
        printk(KERN_INFO "Client: Error: Could not receive message.\n");
        return -1;
    }
    inputBuffer[len] = '\0';
    printk (KERN_INFO "Client: Server replies: %s %d\n",
    inputBuffer, len);

    sock_release(defaultSocket);
    printk(KERN_INFO "Client: Socket connection closed.\n");

    return 0;
}

static int __init initThread(void) {
    socketClient();
    return 0;
}

static void __exit threadCleanup(void) {
    printk(KERN_INFO "Client: Thread clean up.");
}

module_init(initThread);
module_exit(threadCleanup);
