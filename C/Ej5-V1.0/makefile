all: servidor cliente

servidor: servidor.o
	g++ -o servidor servidor.o 
servidor.o: servidor.c
	g++ -c servidor.c

cliente: client.o
	g++ -o cliente client.o 

cliente.o: client.c
	g++ -c client.c


clean: 
	rm *.o 
