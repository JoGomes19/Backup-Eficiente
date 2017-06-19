#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wordexp.h>

typedef struct buffert_t{
	char *data;
	int tam;
	int size;
}BUFFER;

int main(int argc, char *argv[]){
	//CRIA FIFO SE ESTE NÃO EXISTE
	if (access("fifo",F_OK)==-1){
		int f = mkfifo("fifo",0666);
		if(f<0){
			perror("Erro no fifo.\n");
			exit(-1);
		}
	}

	while(1){
		int filedesc=open("fifo",O_RDONLY);
		if(filedesc<0)
			perror("Erro no open.\n");
		//////////////////////////////////////////////
		BUFFER *buffer;
		buffer=(BUFFER*)malloc(sizeof(BUFFER));
		buffer->tam=0;buffer->size=20;
		buffer->data=(char*)malloc((buffer->size)*sizeof(char));
		char c;
		while(read(filedesc,&c,1)>0 && c!='\n'){
			if(buffer->tam==buffer->size){
				BUFFER *aux;
				aux=(BUFFER*)malloc(sizeof(BUFFER));
				aux->tam=buffer->tam;
				aux->size=buffer->size*2;
				aux->data=(char*)malloc(sizeof(char)*(aux->size));
				int i;
				for(i=0;i<buffer->tam;i++){
					aux->data[i]=buffer->data[i];
				}
				free(buffer);
				buffer=aux;
			}
			buffer->data[buffer->tam++]=c;
			//if (c=='\n') printf("OIIII\n");
		}
		c='\n';
		//printf("%s\n",buffer->data);
		if(c=='\n'){
			wordexp_t p;
			char **w;
			wordexp(buffer->data,&p,0);
			w=p.we_wordv;
			buffer->data[buffer->tam]='\0';
			printf("%s\n",buffer->data);
			int k;
			printf("PARTIDO\n");
			for (k=0;k<p.we_wordc;k++){
				printf("%s\n",w[k]);
			}

			//CRIAR DIRECTÓRIO DO BACKUP

			//getcwd

			pid_t criadir=fork();
			if (!criadir){
				//-p se estiver criado não cria novamente
				execlp("mkdir","mkdir","-p","/home/hugo/Backup/",NULL);
			}

			wait(NULL);

			//CRIAR SUB-DIRECTÓRIOS
			pid_t criadata=fork();
			if (!criadata){
				execlp("mkdir","mkdir","-p","/home/hugo/Backup/Data",NULL);
			}

			wait(NULL);

			pid_t criametadata=fork();
			if (!criametadata){
				execlp("mkdir","mkdir","-p","/home/hugo/Backup/MetaData",NULL);
			}

			wait(NULL);

			if (strcmp(w[1],"backup")==0){

				//COMEÇA A COPIAR OS FICHEIROS

				int id=atoi(w[0]);
				printf("%d\n",id);

				for (k=2;k<p.we_wordc;k++){

					//IMPRIME ARGUMENTOS
					//printf("%s\n",w[k]);

					//COPIA FICHEIRO PARA A PASTA BACKUP
					/*
					pid_t filho;
					filho=fork();
					if (!filho){
						execlp("cp","cp",w[k],"/home/hugo/Backup/",NULL);
					}
					*/
					
					//CORRE O PROGRAMA QUE DÁ O DIGEST

					//int ficheiro = open("nome.txt",O_CREAT | O_WRONLY,0644);

					//printf("EIIIIIIIIIIIII\n");

					//ls *.c

					//int ficheiro = open("nome.txt",O_CREAT | O_WRONLY,0644);
					/*
					if (((w[k])[0])=='*'){

					}
					*/

					int fd[2];
					pipe(fd);

					int i=0;

					char *filename = (char *) malloc (sizeof(char)*256);

					pid_t redirect;
					redirect=fork();
					if (!redirect){

						close(fd[0]);
						dup2(fd[1],1);	

						execlp("sha1sum","sha1sum",w[k],NULL);
					}
					else {

						close(fd[1]);

						char c;
						//int bytestotais=0;
						//int tempbytes=0;
						while (read(fd[0],&c,1) && c!=' '){
							//printf("%c\n",c);
							filename[i++]=c;
							//bytestotais+=tempbytes;
						}
						close(fd[0]);

						filename[i]='\0';

						wait(NULL);
					}
					
					//int ficheiroler = open("nome.txt",O_RDONLY,0644);

					//write(1,filename,bytestotais);

					//printf("%s\n",filename);
					
					pid_t filhodata;
					filhodata=fork();
					if(!filhodata){
						execlp("gzip","gzip","-k",w[k],NULL);

					}

					printf("FICHEIRO ZIPADO.\n");

					wait(NULL);

					//sleep(1); //certeza de que move o ficheiro apenas no fim de ser criado

					//RENAME
					
					int tam=strlen(w[k])+3;
					char *actual=(char *) malloc (sizeof(char)*tam);
					strcat(actual,w[k]);
					strcat(actual,".gz");

					int tamm=strlen("/home/hugo/Backup/Data/")+strlen(filename)+3;
					char *final=(char *) malloc (sizeof(char)*tamm);
					strcat(final,"/home/hugo/Backup/Data/");
					strcat(final,filename);
					strcat(final,".gz");

					pid_t movetoData;
					movetoData=fork();
					if(!movetoData){
						execlp("mv","mv",actual,final,NULL);
					}

					free(actual);

					printf("FICHEIRO MOVIDO.\n");

					wait(NULL);

					int tammm=strlen("/home/hugo/Backup/MetaData/")+strlen(w[k]);
					char *argumento=(char *) malloc (sizeof(char)*tammm);
					strcat(argumento,"/home/hugo/Backup/MetaData/");
					strcat(argumento,w[k]);

					
					pid_t links;
					links=fork();
					if(!links){
						//execlp("ln","ln","-s",final,argumento,NULL);
						symlink(final,argumento);
					}

					wait(NULL);

					free(final);

				}

				kill(id,SIGUSR1);

			}

			if (strcmp(w[1],"restore")==0){

				for (k=2;k<p.we_wordc;k++){
					
					int tammmm=strlen("/home/hugo/Backup/MetaData/")+strlen(w[k]);
					char *encontrar=(char *) malloc (sizeof(char)*tammmm);
					strcat(encontrar,"/home/hugo/Backup/MetaData/");
					strcat(encontrar,w[k]);
					
					int fides[2];
					pipe(fides);

					int l=0;

					char *toUnzip = (char *) malloc (sizeof(char)*256);

					pid_t redirectt;
					redirectt=fork();
					if (!redirectt){

						close(fides[0]);
						dup2(fides[1],1);	

						execlp("readlink","readlink",encontrar,NULL);
						//readlink();
					}
					else {

						close(fides[1]);

						char cc;

						while (read(fides[0],&cc,1) && cc!=' '){
							//printf("%c\n",cc);
							toUnzip[l++]=cc;
						}

						close(fides[0]);

						toUnzip[l]='\0';

						wait(NULL);
					}

					//printf("toUnzip:%s",toUnzip);

					toUnzip[strlen(toUnzip)-1]='\0';

					pid_t descomprimir;
					descomprimir=fork();
					if(!descomprimir){
						execlp("gzip","gzip","-d",toUnzip,NULL);
					}

					wait(NULL);

					printf("FICHEIRO DESCOMPRIMIDO.\n");

					char *digestdotoUnzip = (char *) malloc (sizeof(char)*256);

					int index;
					int numerodebarras=0;
					for (index=0;(index<strlen(toUnzip))&&(numerodebarras<5);index++){
						if (toUnzip[index]=='/'){
							numerodebarras++;
						}
					}

					int novoindex=0;

					while (index<strlen(toUnzip)){
						digestdotoUnzip[novoindex]=toUnzip[index];
						index++;
						novoindex++;
					}

					digestdotoUnzip[novoindex]='\0';

					//printf("%s\n",digestdotoUnzip);

					char *digestdigestdotoUnzip = (char *) malloc (sizeof(char)*256);

					int novonovoindex=0;

					while (digestdotoUnzip[novonovoindex]!='.'){
						digestdigestdotoUnzip[novonovoindex]=digestdotoUnzip[novonovoindex];
						novonovoindex++;
					}

					//printf("%s\n",digestdigestdotoUnzip);

					int tamacolocar = strlen("/home/hugo/Área de Trabalho/Trabalho Prático/Código/")+strlen(digestdigestdotoUnzip);
					char *acolocar = (char *) malloc (sizeof(char) * tamacolocar);
					strcat(acolocar,"/home/hugo/Área de Trabalho/Trabalho Prático/Código/");
					strcat(acolocar,digestdigestdotoUnzip);

					int antigo = strlen("/home/hugo/Backup/Data")+strlen(digestdigestdotoUnzip);
					char *acolocarantigo = (char *) malloc (sizeof(char) * antigo);
					strcat(acolocarantigo,"/home/hugo/Backup/Data/");
					strcat(acolocarantigo,digestdigestdotoUnzip);

					pid_t mover;
					mover=fork();
					if(!mover){
						execlp("mv","mv",acolocarantigo,acolocar,NULL);
					}

					wait(NULL);

				}
			}		
					
			if (strcmp(w[1],"delete")==0){

				for (k=2;k<p.we_wordc;k++){

					//printf("%s\n",w[k]);
					
					int tammmmm=strlen("/home/hugo/Backup/MetaData/")+strlen(w[k]);
					char *encontrarpararm=(char *) malloc (sizeof(char)*tammmmm);
					strcat(encontrarpararm,"/home/hugo/Backup/MetaData/");
					strcat(encontrarpararm,w[k]);
					
					int fidescriptor[2];
					pipe(fidescriptor);

					int s=0;

					char *toRemove = (char *) malloc (sizeof(char)*256);

					pid_t redirecttt;
					redirecttt=fork();
					if (!redirecttt){

						close(fidescriptor[0]);
						dup2(fidescriptor[1],1);	

						execlp("readlink","readlink",encontrarpararm,NULL);

					}
					else {

						close(fidescriptor[1]);

						char cccc;
						//int bytestotais=0;
						//int tempbytes=0;
						while (read(fidescriptor[0],&cccc,1) && cccc!=' '){
							//printf("%c\n",cc);
							toRemove[s++]=cccc;
							//bytestotais+=tempbytes;
						}

						close(fidescriptor[0]);

						toRemove[s]='\0';

						wait(NULL);
					}

					int tamanhodotoRemove=strlen(toRemove);
					toRemove[tamanhodotoRemove-1]='\0';

					//printf("toRemove:%s",toRemove);

					pid_t remover;
					remover=fork();
					if(!remover){
						execlp("rm","rm",toRemove,NULL);
					}
					wait(NULL);

					printf("FICHEIRO REMOVIDO.\n");

					int tamRemoveLink = strlen("/home/hugo/Backup/MetaData/")+strlen(w[k]);
					char *RemoveLink = (char *) malloc (sizeof(char) * tamRemoveLink);
					strcat(RemoveLink,"/home/hugo/Backup/MetaData/");
					strcat(RemoveLink,w[k]);

					pid_t removerLink;
					removerLink=fork();
					if(!removerLink){
						execlp("rm","rm",RemoveLink,NULL);
					}
					wait(NULL);

					printf("LINK REMOVIDO.\n");
				}
			}

			//c='\0';
			//printf("%s\n",buffer->data);

			BUFFER *aux;
			aux=(BUFFER*)malloc(sizeof(BUFFER));
			aux->tam=0;aux->size=buffer->size;
			aux->data=(char*)malloc((buffer->size)*sizeof(char));	
			free(buffer);
			buffer=aux;
		}
		close(filedesc);
	}

	return 0;

}

/*
while(true){
	open
	while(readline!=EOF)
}
*/