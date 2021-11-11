#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int create_child(int i, char* n);

void mensaje_ayuda(){
	printf("Se debe desarrollar una implementación de árboles de procesos que cumplan con el grafo.\nCada proceso deberá mostrar toda la ascendencia por ejemplo:\nSoy el pid 1 padres: -\nSoy el pid 2, ascendencia: 1\nSoy el pid 9,padres: 5,2,1\n");
}

int main(int argc,void** arg) { 	
	if(argc>1){
		if(!strcmp(arg[1],"-h"))
			mensaje_ayuda();
		else if(!strcmp(arg[1],"--help"))
			mensaje_ayuda();
		else if(argc>2)
			printf("Cantidad de parámetros inválidos\n");
		else
			printf("Parametros inválidos\n");
		return 0;
	}
	create_child(1, " "); 
	return 0;
}

void printOBO(char* n,int i){
	printf("Este es pid %d padres:",i);
	int x;
	for(x=strlen(n);x>=0;x--)
		printf("%c ",n[x]);
	printf("\n");
}

static int create_child(int i, char* n) {
	pid_t pid;
	char message[strlen(n)+1];
	char aux[1];
	aux[0]=i+'0';
	strcpy(message,n);
	switch(i){
		case 1:
			printOBO("-",i);
			strcat(message,aux);
			pid=fork();	
			if(pid==-1)
				return 1;
			if(pid==0)
				create_child(2,message);	
			else
				create_child(3,message);
			break;
		case 2:	
			printOBO(n,i);
			strcat(message,aux);
			pid=fork();
			if(pid==-1)
				return 1;
			if(pid==0){
				
				create_child(5,message);
			}else
				create_child(4,message);
			break;
		case 4:
			pid=fork();
			if(pid==-1)
				return 1;
			if(pid==0)
				create_child(6,message);
			else
				create_child(7,message);
			break;
		case 3:
			printOBO(n,i);
			strcat(message,aux);
			pid=fork();
			if(pid==0)
				create_child(8,message);
			break;	
		case 7:
			printOBO(n,i);
			strcat(message,aux);
			pid=fork();	
			if(pid==0)
				create_child(10,message);
			else
				create_child(11,message);
			break;	
		case 5:
			printOBO(n,i);
			strcat(message,aux);
			pid=fork();
			if(pid==0)
				create_child(9,message);
			break;
		case 6:
		case 8:
		case 9:
		case 10:
		case 11:
			printOBO(n,i);
			break;

	}
	return 0;
}