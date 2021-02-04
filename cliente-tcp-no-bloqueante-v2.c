/* Archivo: cliente-tcp-no-bloqueante-v2.c 
 *	Abre un socket TCP e inicia una conexión TCP hacia un servidor 
 *	TCP remoto. Una vez conectado, verifica si existen paquetes de
 *	entrada por el socket ó por STDIN.
 *      Si se reciben paquetes por el socket, los envía a STDOUT.
 *      Si se reciben paquetes por STDIN, los envía por el socket.
 **/

/* ARCHIVOS DE CABECERA */
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

/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define SIZE 1024
#define TIME 3600
				   
/* SINONIMOS */
typedef struct sockaddr *sad;

/* FUNCIONES */
void error(char *s){
    perror(s);
    exit(-1);
}

/* FUNCION PRINCIPAL MAIN */
int main(int argc, char **argv){
    int sock, cto; 	//por sock va a iniciar conexion TCP
			//a servidor TCP remoto

    if(argc < 2){
	fprintf(stderr,"usa: %s ipaddr \n", argv[0]);
	exit(-1);
    }
    
    struct sockaddr_in sin; // direccion de internet para sock 
    char linea[SIZE];	// ``buffer´´ de 1024 caracteres (un arreglo)
    fd_set in, in_orig;	// conjuntos de descriptores de ficheros
    struct timeval tv;	// tiempo limite que debe transcurrir antes que
    			// select(2) retorne
    
    if((sock = socket(PF_INET, SOCK_STREAM,0))<0) // abre sock
	    error("socket");

    sin.sin_family=AF_INET;	// familia de direcciones a la que pertenece sin, para sock
    sin.sin_port=htons(PORT);	// puerto de sin, para sock
    
    inet_aton(argv[1],&sin.sin_addr);	
    //direccion de internet de sin, para sock (del servidor remoto)
    /* transforma argv[1] (la direccion IP pasada como argumento al programa) 
    * desde formato ``ascii´´ (puntero a cadena de caracteres) 
    *  al formato ``network´´ (guarda en la ``struct in_addr´´ ``sin.sin_addr´´ )
    *  que a su vez es miembro de la ``struct sockaddr_in´´ ``sin´´.
    *  Se trata de la direccion IP del servidor al que se quiere conectar.
    **/
    
    if( connect(sock, (sad)&sin, sizeof sin)<0)	//sock inicia la conexion TCP
	    error("conect");			// hacia dirección remota sin.sin_addr
    
    /*tiene select*/
    FD_ZERO(&in_orig);	// Limpia el conjunto de descriptores de fichero ``in_orig´´
    FD_SET(0, &in_orig);// añade al conjuto ``in_orig´´ el descriptor STDIN
    FD_SET(sock,&in_orig); // añade al conjunto ``in_orig´´ el descriptor sock

    /*tiene 1 hora*/
    tv.tv_sec=TIME;	//tiempo disponible hasta que select(2) regrese: 3600 segundos
    tv.tv_usec=0;
    
    for(;;){
	memcpy(&in, &in_orig, sizeof in); // copia contenido del conjunto in_orig en in
	if((cto = select(MAX(0,sock)+1,&in,NULL,NULL,&tv))<0)	//observa si hay algo
		error("select");				//en el conjunto in
	if(cto==0)	//si el tiempo de espera para que select(2) retorne expira
		error("timeout");	// error

	/* averiguamos donde hay algo para leer*/
	if(FD_ISSET(0,&in)){	//si hay para leer desde STDIN
	    fgets(linea, SIZE, stdin);	//leer hasta 1024 caracteres de STDIN y ponerlos en linea
	    write(sock, linea, strlen(linea)); //escribir contenido de linea en sock
	}
	if(FD_ISSET(sock,&in)){	//Si hay para leer desde sock
	    cto = read(sock, linea, SIZE); //leer hasta 1024 caracteres de sock y ponerlos en linea
	    if(cto < 0) 
		    error("read");	//si lectura devuelve < 0 ==> error al leer
	    else if(cto==0) break;	//si lectura devuelve 0   ==> parar la ejecucion
	    
	    linea[cto]=0;	//marcar el final del buffer con ``0´´
	    
	    /* Imprime en pantalla la direccion del servidor desde donde vienen datos */
	    printf("\nDe la direccion[ %s ] : puerto [ %d ] --- llega el mensaje:\n",
			    inet_ntoa(sin.sin_addr),
			    ntohs(sin.sin_port));		
	    //Imprime en pantalla lo que vino desde sock1 mediante buffer linea */
	    printf("%s \n",linea);
	}
    }    
close(sock);
return 0;
}
/* Fin Archivo: cliente-tcp-no-bloqueante-v2.c */
