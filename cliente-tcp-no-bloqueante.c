/* Archivo: cliente-tcp-no-bloqueante.c */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>
#include<string.h>
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))

typedef struct sockaddr *sad;

void error(char *s){
    perror(s);
    exit(-1);
}
int main(int argc, char **argv){
    int sock, cto ;
    if(argc<2){
	fprintf(stderr,"usa: %s ipaddr \n", argv[0]);
	exit(-1);
    }
    
    struct sockaddr_in sin;
    char linea[1024];
    fd_set in, in_orig;
    struct timeval tv;
    
    if((sock=socket(PF_INET, SOCK_STREAM,0))<0)
	    error("socket");
    sin.sin_family=AF_INET;
    sin.sin_port=htons(PORT);
    
    inet_aton(argv[1],&sin.sin_addr);
    
    if(connect(sock, (sad)&sin, sizeof sin)<0)
	    error("conect");
    
    /*tiene select*/
    FD_ZERO(&in_orig);
    FD_SET(0, &in_orig);
    FD_SET(sock,&in_orig);
    /*tiene 1 hora*/
    tv.tv_sec=3600;
    tv.tv_usec=0;
    
    for(;;){
	memcpy(&in, &in_orig, sizeof in);
	if((cto=select(MAX(0,sock)+1,&in,NULL,NULL,&tv))<0)
		error("select");
	if(cto==0)
		error("timeout");
	/* averiguamos donde hay algo para leer*/
	if(FD_ISSET(0,&in)){
	    fgets(linea,1024, stdin);
	    write(sock,linea,strlen(linea));
	}
	if(FD_ISSET(sock,&in)){
	    cto=read(sock,linea,1024);
	    if(cto<0) error("read");
	    else if(cto==0) break;
	    
	    linea[cto]=0;
	    printf("%s \n",linea);
	}
    }    
close(sock);
return 0;
}
/* Fin Archivo: cliente-tcp-no-bloqueante.c */
