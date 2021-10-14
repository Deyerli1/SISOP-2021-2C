#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string.h>
#include <dirent.h> 
#include <ctype.h>


typedef struct{
	int watch[10];
	int busy;
}watcher;
watcher Hugin;
DIR* d;
int Munin=0;
int n;


char* readNextFile(){
	struct dirent *dir;
	dir = readdir(d);
	if(dir!= NULL)
		if(strstr(dir->d_name,".txt"))	
			return dir->d_name;
		else
			return readNextFile();
	else
		return NULL;
}

void *servant(void *arg)
{
	int* myturn= (int*)arg;
	long unsigned int myid = pthread_self();
	char* filename;
	FILE *f;
	char target;
	int i;
	do{
		int i,watch[10]={0,0,0,0,0,0,0,0,0,0};
		while(*myturn!=Munin);
		filename=readNextFile();
		if(filename==NULL){
			Munin=(Munin+1)%n;
			break;
		}
		f=fopen(filename,"r");
		while((target=fgetc(f))!=EOF)
			if(isdigit(target))
				watch[target-'0']++;
		printf("Thread %d: [0-%d][1-%d][2-%d][3-%d][4-%d][5-%d][6-%d][7-%d][8-%d][9-%d] en archivo %s\n",*myturn,watch[0],watch[1],watch[2],watch[3],watch[4],watch[5],watch[6],watch[7],watch[8],watch[9],filename);
		while(Hugin.busy);
		Hugin.busy=1;
		for(i=0;i<10;i++)
			Hugin.watch[i]+=watch[i];
		Hugin.busy=0;
		Munin=(Munin+1)%n;
	}while(1);
	return 0;
}
 
int isNumber(char s[])
{
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

int main(int argc,char **argv)
{
	if(argc<2){
		printf("ERROR: Falta parametro\n");
		return 1;
	}else if(!strcmp(argv[1],"-h")||!strcmp(argv[1],"--help")||!strcmp(argv[1],"-?")){
		printf("Se desea desarrollar un programa capaz de contar la cantidad de números del 0 al 9 que aparece en\ntodos los archivos de texto (con extensión .txt) que se encuentra en un determinado directorio. Para\nello deberá recibir un número N que indique el nivel de paralelismo a nivel de threads y el path al\ndirectorio. Cada hilo deberá leer un archivo y contabilizar la cantidad de número que leyó.\nAdicionalmente al final se deberá mencionar la cantidad de números leídos totales.\nEl criterio es que se individualice a cada carácter entre letra y número, en caso de pertenecer al\nconjunto numérico se contabilizará. Por ejemplo la palabra: “1hola451”, sumaría dos apariciones al\nnúmero 1, una aparición al número 4 y una aparición al número 5.\nLa salida por pantalla debe ser:\nThread 1: Archivo leído test.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc\nThread 2: Archivo leído prueba.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc\nThread 1: Archivo leído pepe.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc\n Finalizado lectura: Apariciones total: 0=${cantTotalCeros}, 1=${cantTotalUnos}, etc\n");
		return 1;
	}else if(argc>2){
		printf("ERROR: Demasiados parámetros\n");
		return 1;
	}else if(!isNumber(*argv)){
		printf("ERROR: Parametro no es número\n");
		return 1;
	}
	
	n=atoi(argv[1]);		
	if(n<0){
		printf("ERROR: Cantidad de threads invalido\n");
		return 1;
	}
	pthread_t tid[n];
	
	int i;
	d=opendir(".");
	Hugin.busy=0;
	for(i=0;i<10;i++)
		Hugin.watch[i]=0;

	printf("[Numero-Apariciones]\n");
	for(i=0;i<n;i++){
		int *arg=malloc(sizeof(*arg));
		if( arg==NULL)
			return 1;
		*arg=i;
		pthread_create(&tid[i], NULL, servant,arg);
	}
	for(i=0;i<n;i++)
		pthread_join(tid[i],NULL);
	printf("Resultado final: [0-%d][1-%d][2-%d][3-%d][4-%d][5-%d][6-%d][7-%d][8-%d][9-%d]\n",Hugin.watch[0],Hugin.watch[1],Hugin.watch[2],Hugin.watch[3],Hugin.watch[4],Hugin.watch[5],Hugin.watch[6],Hugin.watch[7],Hugin.watch[8],Hugin.watch[9]);
	pthread_exit(NULL);
	closedir(d);
	return 0;
}
