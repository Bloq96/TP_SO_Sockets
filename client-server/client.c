#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#define SERVER_PORT 54321
#define MAX_LINE 256

int main(int argc, char * argv[])
{
    printf("Inicializando client...\n");
    //FILE *fp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s, len;
    host = "127.0.0.1";
    printf("Host: %s\n", host);
    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0x7F000001);
    sin.sin_port = htons(SERVER_PORT);
    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }
    printf("Conectando ao servidor: %s\n", host);
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
        perror("simplex-talk: connect");
        close(s);
        exit(1);
    }
    
    printf("Conectado ao servidor: %s com sucesso\n", host);
    
    len = 14;
    buf[0] = 'H'; buf[1] = 'e'; buf[2] = 'l'; buf[3] = 'l';
    buf[4] = 'o'; buf[5] = ','; buf[6] = ' '; buf[7] = 's';
    buf[8] = 'e'; buf[9] = 'r'; buf[10] = 'v'; buf[11] = 'e';
    buf[12] = 'r'; buf[13] = '!';
    len = send(s, buf, len, 0);
    printf("Comprimentando o servidor... %d\n", len);

    len = recv(s, buf, sizeof(buf), 0);
    printf("Servidor responde: ");
    for(int it=0;it<len;++it) {
        printf("%c", buf[it]);
    }
    printf(" [%d]\n", len);

    close(s);
}
