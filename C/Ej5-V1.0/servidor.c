#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#define MIN(x,y) (((x)>(y))?(x):(y))
typedef struct s_nodo
{
    void* dato;
    unsigned tambyte;
    struct s_nodo* sig;
} t_nodo;

typedef struct
{
    t_nodo* pri;
    t_nodo* ult;
} t_cola;

typedef struct
{

    char definition[1024];
    char answer[50];
    char used;
} definitions_class;



void crearCola(t_cola *cola);
int colaVacia(const t_cola *cola);
int colaLlena(const t_cola *cola, unsigned tam);
int agregarEnCola(t_cola *cola, void *info, unsigned tam);
int sacarDeCola(t_cola *cola, void *info, unsigned tam);
int TC_parse(definitions_class* def,char* str);



int main(int argc, char const *argv[])
{
    int server_ptr, client_socket,enable=1, client_len, cantRecibida,i,result,puntos,errores=0,cantidadD=0,indice;
    struct sockaddr_in address, client_addr;
    char buffer[1024] = {0};
    char welcome[] = "Por favor, ingresar cantidad de definiciones";
    client_len=sizeof(client_len);
    FILE* defs_ptr;
    definitions_class definitions[50];
    definitions_class tempDefinition;
    char temporal[1075];
    t_cola cola;

    if(argc<2)
    {
        printf("ERROR: Falta parametro\n");
        return 1;
    }
    else if(!strcmp(argv[1],"-h")||!strcmp(argv[1],"--help")||!strcmp(argv[1],"-?"))
    {
        printf("Este es un programa que busca simular de forma online, el juego 'Diccionario' en donde dada unas definiciones, el jugador debera escribir la palabra correcta, recibiendo un punto si lo es, -1 en caso de ser incorrecta y 0 en caso de que decida pasar.\n");
        printf("El parametro que el proceso SERVER recibe es el puerto que se desea usar.Los parametros que el proceso CLIENTE debe recibir es el IP y el puerto, respectivamente\n");
        printf("Para saltear la palabra ingresar el numero 0\n");
    }
    else if(argc>2)
    {
        printf("ERROR: Demasiados parámetros\n");
        return 1;
    }

    defs_ptr=fopen("Definiciones.txt","rt");

    if(!defs_ptr)
    {
        printf("ERROR: No se pudo abrir el archivo de definiciones\n");
        return 1;
    }

    crearCola(&cola);
//// Parseo el archivo
    while( (fgets(temporal,1075,defs_ptr)) )
    {
        TC_parse(&definitions[cantidadD],temporal);
        cantidadD++;
    }


    if ((server_ptr = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("ERROR: No se pudo crear el server\n");
        return 1;
    }

    if (setsockopt(server_ptr, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
    {
        printf("ERROR:No se pudo configurar el server\n");
        return 1;
    }
/// Configuro el server
    memset(&address,0,sizeof(address));
    address.sin_family= AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port =htons(atoi(argv[1])); ////htons(8080); ///

/// Binding
    if( bind(server_ptr,(struct sockaddr *)&address,sizeof(address)) )
    {
        printf("ERROR:No se pudo vincular el server\n");
        return 1;
    }
    while(1)
    {
        sleep(1);

        if (listen(server_ptr,3))
        {
            printf("Muchas conexiones\n");
            return 1;
        }
        printf("Servidor en espera\n");
        if ( (client_socket = accept(server_ptr,(struct sockaddr *)&client_addr,(socklen_t*)&client_len))<0)
        {
            printf("ERROR:No se pudo acceptar la conexion");
            return 1;
        }

        printf("Conexion aceptada\n");
        send(client_socket, welcome, strlen(welcome)+1, 0 );
        recv(client_socket,&cantRecibida,sizeof(int),0);


        if(cantidadD<cantRecibida)
        {
            printf("No existen tantas definiciones, jugando en cambio con %d definiciones\n",cantidadD);
            send(client_socket,&cantidadD,sizeof(int),0);
            cantRecibida=cantidadD;
        }
        else
        {
            send(client_socket,&cantRecibida,sizeof(int),0);
        }

        puntos=0;
        errores=0;
        for (i=0; i<cantRecibida; i++ )
        {
            memset(buffer,0,1024);
            srand(time(NULL));
            indice=rand()%(cantidadD);
            while(definitions[indice].used=='1')
            {
            indice=rand()%(cantidadD);
            }
            send(client_socket, definitions[indice].definition, (strlen(definitions[indice].definition))+1,0);
            definitions[indice].used='1';
            recv(client_socket, buffer, 1024,0);

            /// Si buffer no existe, se salteo la definicion
            if(*buffer=='0')
            {
                agregarEnCola(&cola,&definitions[indice],sizeof(definitions_class));
                errores++;
                printf("%s\n",buffer);
                printf("Se salteo");
                continue;
            }

            result=strcasecmp(buffer,definitions[indice].answer);

            if(result!=0)
            {
                printf("No le pego\n");
                puntos--;
                errores++;
                printf("%s\n",definitions[indice].definition);
                agregarEnCola(&cola,&definitions[indice],sizeof(definitions_class));
                continue;
            }
            else
            {
                printf("Le pego\n");
                puntos++;
            }

        }

/// Termino
        recv(client_socket,&cantRecibida,sizeof(int),0);
        send(client_socket,&errores,sizeof(int),0);

        while(errores--)
        {
            memset(buffer,0,1024);
            sacarDeCola(&cola,&tempDefinition,sizeof(definitions_class));
            strcpy(buffer,tempDefinition.definition);
            printf("%s\n",buffer);
            send(client_socket,buffer,strlen(buffer)+1,0);
            recv(client_socket,&i,sizeof(int),0);
            strcpy(buffer,tempDefinition.answer);
            printf("%s\n",buffer);
            send(client_socket,buffer,strlen(buffer)+1,0);
            recv(client_socket,&i,sizeof(int),0);

        }
        send(client_socket,&puntos,sizeof(int),0);

        printf("Cerrando connection\n");

        for (i=0;i<cantidadD ;i++ )
            {
                definitions[i].used='0';
            }



    }

}



int TC_parse(definitions_class* def,char* str)
{
    char* marker=strrchr(str,'\r');
    if(marker)
    {
        *marker='\0';
    }
    if((marker=strrchr(str,'|'))==NULL)
    {
        return 0;
    }
    strcpy(def->answer,marker+1);
    *marker='\0';

    ///////////////////////////////////////

    strcpy(def->definition,str);
    def->used=0;

    return 1;
}

void crearCola(t_cola *cola)
{
    cola->pri = NULL;
    cola->ult = NULL;
}
int colaVacia(const t_cola *cola)
{
    return (cola->pri == NULL);
}


int agregarEnCola(t_cola *cola, void *info, unsigned tam)
{
    t_nodo *nue = (t_nodo*)malloc(sizeof(t_nodo));
    if(!nue)
        return 0;
    nue->dato = malloc(tam);
    if(!(nue->dato))
    {
        free(nue);
        return 0;
    }
    if(cola->pri == NULL)
        cola->pri = nue;
    else
        cola->ult->sig = nue;

    nue->sig = NULL;
    memcpy(nue->dato,info,tam);
    nue->tambyte = tam; /// o con memcpy?

    cola->ult = nue;

    return 1;

}
int sacarDeCola(t_cola *cola, void *info, unsigned tam)
{
    t_nodo* elim;
    if((cola->pri)==NULL)
        return 0;

    elim=cola->pri;
    memcpy(info,elim->dato,MIN(tam,elim->tambyte));

    cola->pri=elim->sig;
    ///Opcional. Porque me deja la cola cuando se vacia en su situacion inicial
    if(cola->pri == NULL)
        cola->ult = NULL;

    free(elim->dato);
    free(elim);

    return 1;
}
