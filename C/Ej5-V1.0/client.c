#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int sock = 0,cant,i,recibido;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if(argc<3)
    {
        printf("ERROR: Falta parametro\n");
        return 1;
    }
    else if(!strcmp(argv[1],"-h")||!strcmp(argv[1],"--help")||!strcmp(argv[1],"-?"))
    {
        printf("Este es un programa que busca simular de forma online, el juego 'Diccionario' en donde dada unas definiciones, el jugador debera escribir la palabra correcta, recibiendo un punto si lo es, -1 en caso de ser incorrecta y 0 en caso de que decida pasar.\n");
        printf("El parametro que el proceso SERVER recibe es el puerto que se desea usar.Los parametros que el proceso CLIENTE debe recibir es el IP y el puerto, respectivamente\n");
    }
    else if(argc>3)
    {
        printf("ERROR: Demasiados parámetros\n");
        return 1;
    }
    if( ((sock = socket(AF_INET, SOCK_STREAM, 0)) <0) )
    {
        printf("ERROR: No se pudo crear el cliente");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port =htons(atoi(argv[2])); ///htons(8080);

    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) //argv[1] "127.0.0.1"
    {
        printf("ERROR: IP invalido");
        return 1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    recv(sock,buffer,1024,0);

    printf("%s\n",buffer);
    fflush(stdin);
    scanf("%d",&cant);

    send(sock,&cant,sizeof(int),0);

    recv(sock,&recibido,sizeof(int),0);

    if(recibido!=cant)
    {
        printf("No existen tantas definiciones, jugando en cambio con %d definiciones\n",recibido);
        cant=recibido;
    }

    for (i=0; i<cant ; i++ )
    {
        printf("----------------------\n");
        memset(buffer,0,1024);
        recv(sock,buffer,1024,0);
        printf("%s",buffer);
        printf("\nIngresar palabra\n");
        fflush(stdin);

        scanf("%s",buffer);
        send(sock,buffer,strlen(buffer)+1,0);


    }
printf("====================================================================================\n");
    printf("Juego terminado, presiona cualquier tecla para continuar a la puntuacion\n");
printf("====================================================================================\n");

    fflush(stdin);
    scanf("%s",buffer);
    send(sock,buffer,strlen(buffer)+1,0);
    recv(sock,&cant,sizeof(int),0);

    while(cant--)
    {
        printf("====================================================================================\n");
        memset(buffer,0,1024);
        printf("Definicion mal contestada o salteada:\n");
        recv(sock,buffer,1024,0);
        printf("%s\n",buffer);
        i=1;
        send(sock,&i,sizeof(int),0);
    printf("====================================================================================\n");
         printf("Respuesta:\n");
        recv(sock,buffer,1024,0);
        printf("%s\n",buffer);
        i=1;
        send(sock,&i,sizeof(int),0);

    }
    recv(sock,&cant,sizeof(int),0);
    printf("Puntaje total:%d\n",cant);


    return 0;
}
