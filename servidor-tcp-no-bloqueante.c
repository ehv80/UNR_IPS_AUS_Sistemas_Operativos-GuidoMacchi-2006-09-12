/* Archivo: servidor-tcp-no-bloqueante.c
*	Abre un socket TCP y espera la conexión. Una vez establecida,
*	verifica si existen paquetes de entrada por el socket ó por 
*	STDIN.
*	Si se reciben paquetes por el socket, los envía a STDOUT.
*	Si se reciben paquetes por STDIN, los envía por el socket.
*/ 

/* ARCHIVOS DE CABECERA */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>

/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))

typedef struct sockaddr *sad;

/* FUNCIONES */
void error(char *s){
    perror(s);
    exit(-1);
}

/* FUNCION PRINCIPAL MAIN */
int main(){
    int sock, sock1, cto, largo;
    struct sockaddr_in sin, sin1;
    char linea[1024];
    fd_set in, in_orig;
    struct timeval tv;
    
    if((sock=socket(PF_INET, SOCK_STREAM,0))<0)
	    error("socket");
    sin.sin_family=AF_INET;
    sin.sin_port=htons(PORT);
    sin.sin_addr.s_addr= INADDR_ANY;
    
    if(bind(sock, (sad)&sin, sizeof sin)<0)
	    error("bind");
    if(listen(sock,5)<0)
	    error("listen");
    largo=sizeof sin1;
    
    if((sock1=accept(sock,(sad)&sin1,&largo))<0)
	    error("accept");
    /*tiene select*/
    FD_ZERO(&in_orig);
    FD_SET(0, &in_orig);
    FD_SET(sock1, &in_orig);
    /*tiene 1 hora*/
    tv.tv_sec=3600;
    tv.tv_usec=0;
    
    for(;;){
	memcpy(&in, &in_orig, sizeof in);
	if((cto=select(MAX(0,sock1)+1,&in,NULL,NULL,&tv))<0)
		error("select");
	if(cto==0)
		error("timeout");
	/* averiguamos donde hay algo para leer*/
	if(FD_ISSET(0,&in)){
	    fgets(linea,1024, stdin);
	    if( write(sock1,linea,strlen(linea)) < 0 )
		    error("write");
	}
	if(FD_ISSET(sock1,&in)){
	    if( (cto=read(sock1,linea,1024)) < 0 )
		    error("read");
	    else if( cto == 0 )
		    break;
	    linea[cto]=0;
	    printf("%s \n",linea);
	}
    }    
close(sock1);
close(sock);
return 0;
}
/* Fin Archivo: servidor-tcp-no-bloqueante.c */
