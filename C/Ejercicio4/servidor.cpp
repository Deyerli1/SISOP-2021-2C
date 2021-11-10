#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>

using namespace std;

void handlerSenial(int sig)
{
    int idHayCliente = shm_open("memoriaHayCliente", O_CREAT | O_RDWR, 0600);
    char *memoriaHayCliente = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, idHayCliente, 0);
    close(idHayCliente);

    if (*memoriaHayCliente == 'V')
        cout << "No se puede detener el servidor debido a que hay una partida en curso" << endl;
    else
    {
        sem_unlink("semaforo1");
        sem_unlink("semaforo2");
        sem_unlink("semaforoCliente");
        sem_unlink("semaforoConfig");

        shm_unlink("memoriaPuntos");
        shm_unlink("memoriaCantFrases");
        shm_unlink("memoriaCantChr");
        shm_unlink("memoriaMaxFrases");
        shm_unlink("memoriaFrasesIncorrectas");
        shm_unlink("memoriaHayCliente");

        kill(getpid(), SIGTERM);
    }
}

int main()
{
    signal(SIGINT, SIG_IGN);

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = handlerSenial;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &action, NULL);

    vector<string> frases;
    vector<string> definiciones;

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

    int value;
    while (true)
    {
        system("clear");
        cout << "El pid para detener el servidor es: " << getpid() << endl;
        while (sem_wait(semaforoCliente) < 0)
        {
        }

        // Lectura del archivo de definiciones y descripciones

        ifstream myFile("definiciones.txt");
        if (!myFile.is_open())
        {
            cout << "No se puede abrir el archivo" << endl;
            return 0;
        }
        string line;
        vector<string> arch;
        while (getline(myFile, line))
            arch.push_back(line);

        myFile.close();

        *memoriaMaxFrases = arch.size();

        int band0 = 0;
        vector<string> frasesDeArchivo;
        vector<string> definicionesDeArchivo;

        for (int i = 0; i < *memoriaMaxFrases; i++)
        {
            stringstream aux(arch[i]);
            while (getline(aux, line, ','))
            {
                if (band0 == 0)
                {
                    frasesDeArchivo.push_back(line);
                    band0 = 1;
                }
                else
                {
                    definicionesDeArchivo.push_back(line);
                    band0 = 0;
                }
            }
        }

        sem_post(semaforoConfig);
        while (sem_wait(semaforoCliente) < 0)
        {
        }


        // Eligiendo las descripciones que van a utilizarse en el juego

        srand(time(0));
        int indice;
        int puntaje = 0;
        vector<string> incorrectas;

        for (int i = 0; i < *memoriaCantFrases; i++)
        {
            indice = rand() % frasesDeArchivo.size();
            frases.push_back(frasesDeArchivo[indice]);
            definiciones.push_back(definicionesDeArchivo[indice]);
            frasesDeArchivo.erase(frasesDeArchivo.begin() + indice);
            definicionesDeArchivo.erase(definicionesDeArchivo.begin() + indice);
        }

        int cantChr;
        while (!frases.empty() && *memoriaHayCliente == 'V')
        {
            cantChr = frases[0].size();
            char fraseEnCurso[cantChr + 1];
            strcpy(fraseEnCurso, frases[0].c_str());

            *memoriaCantChr = cantChr;

            int idMemoriaFrase = shm_open("memoriaFrase", O_CREAT | O_RDWR, 0600);
            ftruncate(idMemoriaFrase, sizeof(char) * cantChr);
            char *memoriaFrase = (char *)mmap(NULL, sizeof(char) * cantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaFrase, 0);
            close(idMemoriaFrase);

            for (int i = 0; i < cantChr; i++)
                *(memoriaFrase + i) = fraseEnCurso[i];

            sem_post(semaforo1);
            while (sem_wait(semaforo2) < 0)
            {
            }

            string palabraCliente = "";

            if (*memoriaHayCliente == 'V')
            {

                // Recibiendo la respuesta del jugador...

                int idMemoriaPalabra = shm_open("memoriaPalabra", O_CREAT | O_RDWR, 0600);
                ftruncate(idMemoriaPalabra, sizeof(char) * *memoriaCantChr);
                char *memoriaPalabra = (char *)mmap(NULL, sizeof(char) * *memoriaCantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaPalabra, 0);
                close(idMemoriaPalabra);

                for (int i = 0; i < *memoriaCantChr; i++)
                    palabraCliente += *(memoriaPalabra + i);                

                if (palabraCliente.size() == 0)
                {
                    incorrectas.push_back(frases[0]);
                    incorrectas.push_back(definiciones[0]);                    
                }
                else if (definiciones[0] == palabraCliente)
                {
                    puntaje++;
                }
                else
                {
                    incorrectas.push_back(frases[0]);
                    incorrectas.push_back(definiciones[0]);
                    puntaje--;
                }

                munmap(memoriaPalabra, sizeof(char) * *memoriaCantChr);
                shm_unlink("memoriaPalabra");
            }
            else if (*memoriaHayCliente == 'C')
                frases.clear();

            munmap(memoriaFrase, sizeof(char) * cantChr);
            shm_unlink("memoriaFrase");
            frases.erase(frases.begin());
            definiciones.erase(definiciones.begin());
        }
        *memoriaPuntos = puntaje;

        // Calculando puntaje final

        if (puntaje != *memoriaCantFrases)
        {
            cantChr = 0;
            for (int i = 0; i < incorrectas.size(); i++)
                cantChr += incorrectas[i].size() + 1;

            *memoriaCantChr = cantChr - 1;

            cout << *memoriaCantChr << endl;

            int idMemoriaFrasesIncorrectas = shm_open("memoriaFrasesIncorrectas", O_CREAT | O_RDWR, 0600);
            ftruncate(idMemoriaFrasesIncorrectas, sizeof(char) * *memoriaCantChr);
            char *memoriaFrasesIncorrectas = (char *)mmap(NULL, sizeof(char) * *memoriaCantChr, PROT_READ | PROT_WRITE, MAP_SHARED, idMemoriaFrasesIncorrectas, 0);
            close(idMemoriaFrasesIncorrectas);

            // Serializando el listado de rondas saltadas o perdidas para cargar en memoria

            int cont = 0;
            for (int i = 0; i < incorrectas.size(); i += 2)
            {
                cantChr = incorrectas[i].size();
                char frase[cantChr + 1];
                strcpy(frase, incorrectas[i].c_str());

                for (int j = 0; j < incorrectas[i].size(); j++)
                {
                    *(memoriaFrasesIncorrectas + cont) = frase[j];
                    cont++;
                }
                *(memoriaFrasesIncorrectas + cont) = ',';
                cont++;
                cantChr = incorrectas[i + 1].size();
                char definicion[cantChr + 1];
                strcpy(definicion, incorrectas[i + 1].c_str());

                for (int j = 0; j < incorrectas[i + 1].size(); j++)
                {
                    *(memoriaFrasesIncorrectas + cont) = definicion[j];
                    cont++;
                }
                *(memoriaFrasesIncorrectas + cont) = ',';
                cont++;
            }
        }
        *memoriaHayCliente = 'F';
        sem_post(semaforo1);
    }

    return EXIT_SUCCESS;
}