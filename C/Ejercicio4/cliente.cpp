#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sstream>

using namespace std;

void handlerSenial(int sig)
{
    sem_t *semaforo2 = sem_open("semaforo2", O_CREAT, 0600, 0);

    int idHayCliente = shm_open("memoriaHayCliente", O_CREAT | O_RDWR, 0600);
    char *memoriaHayCliente = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, idHayCliente, 0);
    close(idHayCliente);

    *memoriaHayCliente = 'C';
    sem_post(semaforo2);
    kill(getpid(), SIGTERM);
}

int main()
{
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = handlerSenial;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGINT, &action, NULL);

    sem_t *semaforo1 = sem_open("semaforo1", O_CREAT, 0600, 0);
    sem_t *semaforo2 = sem_open("semaforo2", O_CREAT, 0600, 0);
    sem_t *semaforoCliente = sem_open("semaforoCliente", O_CREAT, 0600, 0);
    sem_t *semaforoConfig = sem_open("semaforoConfig", O_CREAT, 0600, 0);

    int idMemoriaPuntos = shm_open("memoriaPuntos", O_CREAT | O_RDWR, 0600);
    int idMemoriaCantFrases = shm_open("memoriaCantFrases", O_CREAT | O_RDWR, 0600);
    int idMemoriaCantChr = shm_open("memoriaCantChr", O_CREAT | O_RDWR, 0600);
    int idMemoriaMaxFrases = shm_open("memoriaMaxFrases", O_CREAT | O_RDWR, 0600);
    int idHayCliente = shm_open("memoriaHayCliente", O_CREAT | O_RDWR, 0600);

    ftruncate(idMemoriaPuntos, sizeof(int));
    ftruncate(idMemoriaCantFrases, sizeof(int));
    ftruncate(idMemoriaCantChr, sizeof(int));
    ftruncate(idMemoriaMaxFrases, sizeof(int));
    ftruncate(idHayCliente, sizeof(char));

    int *memoriaPuntos = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaPuntos, 0);
    int *memoriaCantFrases = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaCantFrases, 0);
    int *memoriaCantChr = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaCantChr, 0);
    int *memoriaMaxFrases = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaMaxFrases, 0);
    char *memoriaHayCliente = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, idHayCliente, 0);

    close(idMemoriaPuntos);
    close(idMemoriaCantFrases);
    close(idMemoriaCantChr);
    close(idMemoriaMaxFrases);
    close(idHayCliente);

    string definicion = "";
    int value;
    *memoriaHayCliente = 'V';
    int cantFrases = INT32_MAX;
    sem_post(semaforoCliente);
    while (sem_wait(semaforoConfig) < 0)
    {
        cout << "Cargando...";
        system("clear");
    }
    while (cantFrases > *memoriaMaxFrases)
    {
        cout << "Ingrese la cantidad de frases que desea adivinar siendo " << *memoriaMaxFrases << " el maximo posible: ";
        cin >> cantFrases;
    }

    *memoriaCantFrases = cantFrases;
    sem_post(semaforoCliente);
    cin.ignore();
    while (*memoriaHayCliente == 'V')
    {
        sem_wait(semaforo1);

        if (*memoriaHayCliente == 'V')
        {
            // leyendo la respuesta del usuario

            int idMemoriaFrase = shm_open("memoriaFrase", O_CREAT | O_RDWR, 0600);
            ftruncate(idMemoriaFrase, sizeof(char) * *memoriaCantChr);
            char *memoriaFrase = (char *)mmap(NULL, sizeof(char) * *memoriaCantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaFrase, 0);
            close(idMemoriaFrase);

            cout << "\nLa frase es: ";
            for (int i = 0; i < *memoriaCantChr; i++)
                cout << *(memoriaFrase + i);

            cout << "\nIngrese la definición: ";               
            getline(cin, definicion);            

            munmap(memoriaFrase, sizeof(char) * *memoriaCantChr);
            *memoriaCantChr = definicion.size();

            int idMemoriaPalabra = shm_open("memoriaPalabra", O_CREAT | O_RDWR, 0600);
            ftruncate(idMemoriaPalabra, sizeof(char) * *memoriaCantChr);
            char *memoriaPalabra = (char *)mmap(NULL, sizeof(char) * *memoriaCantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaPalabra, 0);
            close(idMemoriaPalabra);
            
            // mapeando la respuesta a memoria

            char definicionIngresada[*memoriaCantChr + 1];
            strcpy(definicionIngresada, definicion.c_str());

            for (int i = 0; i < *memoriaCantChr; i++)
                *(memoriaPalabra + i) = definicionIngresada[i];
            
            munmap(memoriaPalabra, sizeof(char) * *memoriaCantChr);
            sem_post(semaforo2);
        }
    }

    // Fin de la partida - Informe del juego

    cout << "\n\n";
    if (cantFrases == *memoriaPuntos)
    {
        cout << "Felicidades, obtuvo el puntaje perfecto de: " << *memoriaPuntos << endl;
    }
    else
    {
        int idMemoriaFrasesIncorrectas = shm_open("memoriaFrasesIncorrectas", O_CREAT | O_RDWR, 0600);
        ftruncate(idMemoriaFrasesIncorrectas, sizeof(char) * *memoriaCantChr);
        char *memoriaFrasesIncorrectas = (char *)mmap(NULL, sizeof(char) * *memoriaCantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaFrasesIncorrectas, 0);
        close(idMemoriaFrasesIncorrectas);

        cout << "Su puntuacion final es: " << *memoriaPuntos << endl;
        
        cout << "\nLas definiciones erroneas fueron: " << endl;
        
        // deserializando el listado de rondar saltadas o perdidas desde la memoria

        int band = 0;
        for (int i = 0; i < *memoriaCantChr; i++)
        {
            if (*(memoriaFrasesIncorrectas + i) == ',' && band == 0)
            {
                cout << "-->";
                band = 1;
            }
            else if (*(memoriaFrasesIncorrectas + i) == ',' && band == 1)
            {
                cout << endl;
                band = 0;
            }
            else
            {
                cout << *(memoriaFrasesIncorrectas + i);
            }            
        }
        cout << endl;
        munmap(memoriaFrasesIncorrectas, sizeof(char) * *memoriaCantChr);
    }

    munmap(memoriaPuntos, sizeof(int));
    munmap(memoriaCantFrases, sizeof(int));
    munmap(memoriaCantChr, sizeof(int));
    munmap(memoriaMaxFrases, sizeof(int));

    sem_close(semaforo1);
    sem_close(semaforo2);
    sem_close(semaforoCliente);

    return EXIT_SUCCESS;
}