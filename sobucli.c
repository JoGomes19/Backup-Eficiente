#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void copiado(){
	write(1,"Fim.",strlen("Fim."));
}

int main(int argc, char *argv[]){

	//ABRE O FIFO
	int filedesc=open("fifo",O_WRONLY);

	if(filedesc<0){
		perror("Erro no open.\n");
		exit(-1);
	}

	int i,j;

	if (argc>=3 && strcmp(argv[1],"backup")==0){

		//printf("ENTREI NOS ARGUMENTOS\n");
		//printf("%d\n",argc);
		
		char *identidade = (char *) malloc (sizeof(char)*256);
		printf("%d\n",getpid());
		sprintf(identidade,"%d",getpid());
		printf("%s\n",identidade);
		write(filedesc,identidade,strlen(identidade));
		write(filedesc," ",1);
		
		for(i=1;i<argc;i++){
			write(filedesc,argv[i],strlen(argv[i]));
			write(filedesc," ",1);
			
			if (i>1){
				write(1,argv[i],strlen(argv[i]));
				write(1,": copiado",strlen(": copiado"));
				write(1,"\n",strlen("\n"));
			}
			
		}
		signal(SIGUSR1,copiado);
	}

	if (argc>=3 && strcmp(argv[1],"restore")==0){

		//printf("ENTREI NOS ARGUMENTOS\n");
		//printf("%d\n",argc);

		char *identidadeRestore = (char *) malloc (sizeof(char)*256);
		printf("%d\n",getpid());
		sprintf(identidadeRestore,"%d",getpid());
		printf("%s\n",identidadeRestore);
		write(filedesc,identidadeRestore,strlen(identidadeRestore));
		write(filedesc," ",1);

		for(i=1;i<argc;i++){
			write(filedesc,argv[i],strlen(argv[i]));
			write(filedesc," ",1);
			if (i>1){
				write(1,argv[i],strlen(argv[i]));
				write(1,": recuperado",strlen(": recuperado"));
				write(1,"\n",strlen("\n"));
			}
		}
	}

	if (argc>=3 && strcmp(argv[1],"delete")==0){

		//printf("ENTREI NOS ARGUMENTOS\n");
		//printf("%d\n",argc);

		char *identidadeDelete = (char *) malloc (sizeof(char)*256);
		printf("%d\n",getpid());
		sprintf(identidadeDelete,"%d",getpid());
		printf("%s\n",identidadeDelete);
		write(filedesc,identidadeDelete,strlen(identidadeDelete));
		write(filedesc," ",1);

		for(i=1;i<argc;i++){
			write(filedesc,argv[i],strlen(argv[i]));
			write(filedesc," ",1);
			if (i>1){
				write(1,argv[i],strlen(argv[i]));
				write(1,": apagado",strlen(": apagado"));
				write(1,"\n",strlen("\n"));
			}
		}
	}

	close(filedesc);

	exit(0);
}