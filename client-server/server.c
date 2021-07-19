#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#define SERVER_PORT 54321
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
    printf("Inicializando servidor...\n");
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    char symbol;
    unsigned int len = sizeof(sin);
    int s, new_s;
    /* build address data structure */
    bzero((char *)&sin, len);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);
    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }
    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    }
    listen(s, MAX_PENDING);

    //new_s = accept(s, (struct sockaddr *)&sin, &len);
    //while(1) {
        printf("Aguardando conexão do client\n");
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            printf("Erro na tentativa de conexão do client\n");
            perror("simplex-talk: accept");
            exit(1);
        }
        printf("Client conectado\n");
        printf("Digite o que deseja enviar:\n");
	len = 0;
	scanf("%c",&symbol);
        while(symbol!='\n'&&len<256) {
	    buf[len]=symbol;
	    scanf("%c",&symbol);
	    ++len;
        }
        buf[len] = '\0';
        send(new_s, buf, len, 0);
        printf("Mensagem enviada.\n");
	while(recv(new_s,buf,sizeof(buf),0));
        close(new_s);
        close(s);
    //}
}
