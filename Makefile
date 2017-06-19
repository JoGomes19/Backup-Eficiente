#############################
#Para escrever comentários ##
############################# Makefile ##########################
all: sobusrv sobucli

sobusrv: sobusrv.o	
	gcc -o sobusrv sobusrv.o

sobusrv.o: sobusrv.c
	gcc -Wall -c sobusrv.c 

sobucli:sobucli.o
	gcc -o sobucli sobucli.o

sobucli.o:sobucli.c 
	gcc -Wall -c sobucli.c 

clean:
	rm -rf *.o
mrproper:clean
	rm -rf teste
