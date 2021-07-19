#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s, len;
    host = "192.168.100.1"; 
    host = "10.0.2.15";
    host = "127.0.0.1";
    printf("Host: %s\n", host);
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
        exit(1);
    }
    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
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
    len = recv(s, buf, sizeof(buf), 0);
    printf("Servidor diz: ");
    for(int it=0;it<len;++it) {
        printf("%c", buf[it]);
    }
    printf("\n");

    close(s);
}
