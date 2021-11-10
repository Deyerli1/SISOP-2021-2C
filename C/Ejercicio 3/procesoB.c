// Actividad Práctica de Laboratorio Nro. 3 - Ejercicio 3
// Fierro, Agustin Gabriel- 42.427.695
// Albanesi, Matias - 39.770.388
// Rodriguez, Ezequiel Nicolás - 40.135.570
// Jimenez Vitale, Matias - 34.799.834
// Cambiasso, Tomas - 41.471.465

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <sys/file.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdbool.h>

#define TAMBUF 1024 * 42
#define PID_FILE "/tmp/server"

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
resultado_proceso calcularTotales(viajes data[], int index_max, char *chofer);
resultado_proceso avg_valor(viajes data[], int index_max);
resultado_proceso list(viajes data[], int index_max, char *chofer);
resultado_proceso max_gasoil(viajes data[], int index_max);

int main(int argc, char *argv[])
{
    infoProceso info;
    int r, w;
    resultado_proceso resultado;
    char archivo[TAMBUF];
    bool fin = false;

    int index = 0;
    char b[1024];
    FILE *file;
    viajes row;
    viajes data[100];

    if (argc < 1)
    {
        printf("Inserte un párametro, -h o --help para obtener ayuda.\n");
        exit(0);
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        mostrarAyuda();
        exit(1);
    }

    int F = open(PID_FILE, O_CREAT | O_RDWR, 0666);
    int rc = flock(F, LOCK_EX | LOCK_NB);

    if (rc)
    {
        if (EWOULDBLOCK == errno)
        {
            printf("Ya se esta corriendo otra instancia!\n");
            exit(0);
        }
    }

    // printf("Cantidad de parametros: %d\n", argc);
    // printf("Parametro 0: %s\n", argv[0]);

    strcpy(archivo, argv[1]);
    // printf("Archivo pasado por parametro %s\n", archivo);

    file = fopen(archivo, "r");
    if (!file)
    {
        printf("Error al leer archivo. El archivo %s no existe.\n", archivo);
        exit(1);
    }

    memset(b, 0, 1024);
    fgets(b, 1024, file);
    while (!feof(file))
    {
        fgets(b, 1024, file);
        sscanf(b, "%d\t%s\t%s\t%d\t%d\t%d", &row.id, row.chofer, row.origenDestino, &row.kms, &row.gasoil, &row.valor);
        data[index].id = row.id;
        data[index].kms = row.kms;
        data[index].gasoil = row.gasoil;
        data[index].valor = row.valor;
        strcpy(data[index].chofer, row.chofer);
        strcpy(data[index].origenDestino, row.origenDestino);
        index++;
    }
    fclose(file);

    // for (int i = 0; i < index; i++)
    // {
    //     printf("%-5d %-20s %-15s %-10d %-5d %-5d\n", data[i].id, data[i].chofer, data[i].origenDestino, data[i].kms, data[i].gasoil, data[i].valor);
    // }

    // Evita que lo corte ctrl + c
    signal(SIGINT, SIG_IGN);

    do
    {
        r = open("./fifo1", O_RDONLY);
        w = open("./fifo2", O_WRONLY);

        read(r, &info, sizeof(info));

        // printf("\n");
        // printf("Datos recibidos ...\n");
        // printf("Opcion: %s\n", info.opcion);
        // if (strlen(info.filtro) > 0)
        // {
        //      printf("Filtro: %s\n", info.filtro);
        // }

        if (strstr(info.opcion, "SUM") != NULL)
        {
            //printf("Resolviendo SUM\n");
            resultado = calcularTotales(data, index, info.filtro);
            write(w, &resultado, sizeof(resultado));
        }
        else if (strstr(info.opcion, "MAX_GASOIL") != NULL)
        {
            //printf("Resolviendo MAX GASOIL\n");
            resultado = max_gasoil(data, index);
            write(w, &resultado, sizeof(resultado));
        }
        else if (strstr(info.opcion, "AVG_VALOR") != NULL)
        {
            //printf("Resolviendo AVG\n");
            resultado = avg_valor(data, index);
            write(w, &resultado, sizeof(resultado));
        }
        else if (strstr(info.opcion, "LIST") != NULL)
        {
            //printf("Resolviendo LIST\n");
            resultado = list(data, index, info.filtro);
            write(w, &resultado, sizeof(resultado));
        }
        else if (strstr(info.opcion, "QUIT") != NULL)
        {
            //printf("Resolviendo QUIT\n");
            //resultado = list(data, index, info.filtro);
            write(w, &resultado, sizeof(resultado));
            close(r);
            close(w);
            fin = true;
        }

        //printf("Enviando respuesta ...\n");
        //write(w, &resultado, sizeof(resultado));

    } while (!fin);

    // close(r);
    // close(w);

    printf("Programa SERVIDOR finalizado correctamente.\n");
    return 0;
}

void mostrarAyuda()
{
    printf("El Proceso Servidor (procesoB) recibirá por parametro el nombre de archivo con los viajes a leer/procesar.\n");
    printf("Mediante una tuberia, leerá los comandos procesados en el Proceso Cliente (procesoa), y ejecutara las distintas consultas solicitadas e informara por medio de otra tuberia que estará escuchando el Proceso Cliente.\n");
    printf("Ejemplo de ejecución.:\n");
    printf("./procesoB ./2021.viajes\n");
    exit(1);
}

resultado_proceso calcularTotales(viajes data[], int index_max, char *chofer)
{
    //printf("\n\nCHOFER POR PARAMETRO: %s\n", chofer);

    chofer[strlen(chofer) - 1] = '\0';

    resultado_proceso result;

    for (int i = 0; i < index_max; i++)
    {
        int p = strcmp(data[i].chofer, chofer) == 0;

        if (p == 1)
        {
            result.total_kms += data[i].kms;
            result.total_valor += data[i].valor;
            result.total_gasoil += data[i].gasoil;
        }
    }

    // printf("\n----------TOTALES--------------");
    // printf("\nKms: %d", result.total_kms);
    // printf("\nGasoil: %d Litros", result.total_gasoil);
    // printf("\nValor: %d\n\n", result.total_valor);
    return result;
}

resultado_proceso avg_valor(viajes data[], int index_max)
{
    int total_valor = 0;
    resultado_proceso result;
    float avg = 0;

    for (int i = 0; i < index_max; i++)
    {
        //printf("%-5d %-20s %-15s %-10d %-5d %-5d\n", data[i].id, data[i].chofer, data[i].origenDestino, data[i].kms, data[i].gasoil, data[i].valor);
        //printf("Linea %d: %d\n", i, data[i].gasoil);
        total_valor += data[i].valor;
    }

    result.avg = total_valor / index_max;

    //printf("\n----------AVG CALCULADO--------------");
    //printf("\nAVG: %.0f\n", result.avg);
    return result;
}

resultado_proceso list(viajes data[], int index_max, char *chofer)
{
    //printf("\n\nCHOFER POR PARAMETRO: %s", chofer);

    chofer[strlen(chofer) - 1] = '\0';

    int filtrados = 0;
    resultado_proceso result;

    for (int i = 0; i < index_max; i++)
    {
        int p = strcmp(data[i].chofer, chofer) == 0;

        if (p == 1)
        {
            result.datos_filtrados[filtrados].id = data[i].id;
            result.datos_filtrados[filtrados].gasoil = data[i].gasoil;
            result.datos_filtrados[filtrados].valor = data[i].valor;
            result.datos_filtrados[filtrados].kms = data[i].kms;
            strcpy(result.datos_filtrados[filtrados].chofer, data[i].chofer);
            strcpy(result.datos_filtrados[filtrados].origenDestino, data[i].origenDestino);
            filtrados++;
        }
    }

    result.filtrados = filtrados;

    //printf("\nFILTRADOS: %d\n", result.filtrados);
    // printf("\n---------DATOS FILTRADOS-------------------");
    // for (int k = 0; k < result.filtrados; k++)
    // {
    //     int j = 0;
    //     char str[2][100];
    //     char *token = strtok(result.datos_filtrados[k].origenDestino, "-");
    //     while (token != NULL)
    //     {
    //         strcpy(str[j], token);
    //         token = strtok(NULL, "-");
    //         j++;
    //     }

    //     printf("\n%d - %s -> %s ( %d Km ) - %d Lt - $ %d", result.datos_filtrados[k].id,
    //            str[0],
    //            str[1],
    //            result.datos_filtrados[k].kms,
    //            result.datos_filtrados[k].gasoil,
    //            result.datos_filtrados[k].valor);
    // }
    // printf("\n\n");

    return result;
}

resultado_proceso max_gasoil(viajes data[], int index_max)
{
    resultado_proceso result;
    int row = 0;
    int height = 0;

    for (int i = 0; i < index_max; i++)
    {
        int temp = data[i].gasoil;
        if (temp > height)
        {
            height = temp;
            row = i;
        }
    }

    //printf("\nMAXIMO %d", row);

    result.max_item.id = data[row].id;
    result.max_item.kms = data[row].kms;
    result.max_item.gasoil = data[row].gasoil;
    result.max_item.valor = data[row].valor;
    strcpy(result.max_item.chofer, data[row].chofer);
    strcpy(result.max_item.origenDestino, data[row].origenDestino);

    // printf("\nID: %d\n", result.max_item.id);
    // printf("Chofer: %s\n", result.max_item.chofer);
    // printf("Trayecto: %s ( %d Km )\n", result.max_item.origenDestino, result.max_item.kms);
    // printf("Gasoil: %d Lt\n", result.max_item.gasoil);
    // printf("\n");

    return result;
}