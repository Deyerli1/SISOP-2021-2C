// Actividad Práctica de Laboratorio Nro. 3 - Ejercicio 3
// Fierro, Agustin Gabriel- 42.427.695
// Albanesi, Matias - 39.770.388
// Rodriguez, Ezequiel Nicolás - 40.135.570
// Jimenez Vitale, Matias - 34.799.834
// Cambiasso, Tomas - 41.471.465

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>

#define TAMBUF 1024 * 42
#define PID_FILE "/tmp/cliente"

typedef struct
{
    char opcion[50];
    char filtro[50];

} infoProceso;

typedef struct
{
    int id;
    char chofer[200];
    char origenDestino[200];
    int kms;
    int gasoil;
    int valor;

} viajes;

typedef struct
{
    int total_kms;
    int total_gasoil;
    int total_valor;
    float avg;
    viajes datos_filtrados[100];
    int filtrados;
    viajes max_item;

} resultado_proceso;

void mostrarAyuda();

int main(int argc, char *argv[])
{
    int w, r = -1;
    infoProceso info;
    resultado_proceso resultado;
    char teclado[100];
    bool fin = false;
    bool ok = true;

    // printf("Cantidad de parametros: %d\n", argc);
    // printf("Parametro 0: %s\n", argv[0]);
    // printf("Parametro 1: %s\n", argv[1]);
    // printf("Parametro 2: %s\n", argv[2]);

    // if (argc < 1)
    // {
    //     printf("Inserte un párametro, -h o --help para obtener ayuda.\n");
    //     exit(0);
    // }

    if (argc == 2 && (strstr(argv[1], "-h") != NULL || strstr(argv[1], "--help") != NULL))
    {
        mostrarAyuda();
        exit(1);
    }

    int f = open(PID_FILE, O_CREAT | O_RDWR, 0666);
    int rc = flock(f, LOCK_EX | LOCK_NB);

    if (rc)
    {
        if(EWOULDBLOCK == errno)
        {
            printf("Ya se esta corriendo otra instancia!\n");
            exit(0);
        }
    }

    // Evita que lo corte ctrl + c
    signal(SIGINT, SIG_IGN);

    do
    {
        ok = true;
        printf("Ingrese comando: ");
        fgets(teclado, 100, stdin);

        int l = 0;
        char strr[2][100];
        char *tokenn = strtok(teclado, " ");
        while (tokenn != NULL)
        {
            strcpy(strr[l], tokenn);
            tokenn = strtok(NULL, " ");
            l++;
        }

        strcpy(info.opcion, strr[0]);
        if (strr[1] != NULL && strlen(strr[1]) > 0)
        {
            strcpy(info.filtro, strr[1]);
        }

        // strcpy(info.opcion, argv[1]);
        // if (argv[2] != NULL && strlen(argv[2]) > 0)
        // {
        //     strcpy(info.filtro, argv[2]);
        // }

        bool condicion = strstr(info.opcion, "SUM") != NULL || strstr(info.opcion, "LIST") != NULL || strstr(info.opcion, "MAX_GASOIL") != NULL || strstr(info.opcion, "AVG_VALOR") != NULL || strstr(info.opcion, "QUIT") != NULL;

        if (condicion == false)
        {
            printf("La opcion ingresa es INCORRECTA. Consulte la ayuda del programa con -h o --help\n");
            ok = false;
            //exit(1);
        }

        if (strstr(info.opcion, "SUM") != NULL || strstr(info.opcion, "LIST") != NULL)
        {
            if (info.filtro == NULL || strlen(info.filtro) == 0)
            {
                printf("Debe ingresar un 2do parametro (nombre de chofer)\n");
                ok = false;
                //exit(1);
            }
        }

        if (ok == true)
        {
            //printf("Esperando tuberia...\n\n");
            mkfifo("./fifo1", 0666);
            mkfifo("./fifo2", 0666);
            w = open("./fifo1", O_WRONLY);
            write(w, &info, sizeof(info));

            r = open("./fifo2", O_RDONLY);
            //printf("ESPERANDO READ...\n\n");
            read(r, &resultado, sizeof(resultado));

            if (strstr(info.opcion, "SUM") != NULL)
            {
                printf("\nKms: %d", resultado.total_kms);
                printf("\nGasoil: %d Litros", resultado.total_gasoil);
                printf("\nValor: $ %d\n\n", resultado.total_valor);
            }
            else if (strstr(info.opcion, "MAX_GASOIL") != NULL)
            {
                printf("\nID: %d\n", resultado.max_item.id);
                printf("Chofer: %s\n", resultado.max_item.chofer);
                printf("Trayecto: %s ( %d Km )\n", resultado.max_item.origenDestino, resultado.max_item.kms);
                printf("Gasoil: %d Lt\n\n", resultado.max_item.gasoil);
            }
            else if (strstr(info.opcion, "AVG_VALOR") != NULL)
            {
                printf("\n$ %.0f\n\n", resultado.avg);
            }
            else if (strstr(info.opcion, "LIST") != NULL)
            {
                for (int k = 0; k < resultado.filtrados; k++)
                {
                    int j = 0;
                    char str[2][100];
                    char *token = strtok(resultado.datos_filtrados[k].origenDestino, "-");
                    while (token != NULL)
                    {
                        strcpy(str[j], token);
                        token = strtok(NULL, "-");
                        j++;
                    }

                    printf("\n%d - %s -> %s ( %d Km ) - %d Lt - $ %d", resultado.datos_filtrados[k].id,
                           str[0],
                           str[1],
                           resultado.datos_filtrados[k].kms,
                           resultado.datos_filtrados[k].gasoil,
                           resultado.datos_filtrados[k].valor);
                }
                printf("\n\n");
            }
            else if (strstr(info.opcion, "QUIT") != NULL)
            {
                close(r);
                close(w);

                unlink("./fifo1");
                unlink("./fifo2");

                fin = true;
            }
        }

    } while (!fin);

    // close(r);
    // close(w);

    // unlink("./fifo1");
    // unlink("./fifo2");

    printf("Programa CLIENTE finalizado correctamente.\n");
    return 0;
}

void mostrarAyuda()
{
    printf("El Proceso Cliente (procesoA) recibira por parámetro los comandos y los enviara al Proceso Servidor (procesoB).\n");
    printf("Por medio de una tuberia FIFO, esperará respuesta e informará por consola el resultado.\n");
    printf("Comandos de ingreso.:\n");
    printf("1- SUM Hirschfeldt (Muestra el total de Km, Gasoil y Valor de viaje para un chofer dado)\n");
    printf("2- MAX_GASOIL (Muestra información sobre el viaje que mas consumo de Gasoil tuvo).\n");
    printf("3- AVG_VALOR (Muestra el promedio de Valor de viaje para todo el archivo).\n");
    printf("4- LIST Hirschfeldt (Muestra una lista de los viajes realizados por un chofer).\n");
    exit(1);
}