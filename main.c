#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <time.h>
#define np 5
#define nc 2

#define scab 0
#define mutex 1
#define span 2

int sem,memp,p,err;
int *mp;
key_t clef;

int main(){
       /*Création de sémaphores*/
        clef = ftok("main.c",2); 
	sem = semget(clef,3,IPC_CREAT | 0666);
	if(sem == -1){
	  printf("Erreur de cretation semaphores \n");
	  exit(1);
	}  
	
	/*Initialisation de mémoire partagé*/
	semctl(sem,scab,SETVAL,nc);
        semctl(sem,mutex,SETVAL,1);
        semctl(sem,span,SETVAL,0);
	/*Création de la mémoire partagée*/
	clef = ftok("main.c",3);
	memp = shmget(clef,2*sizeof(int),IPC_CREAT | 0666);
	if(memp == -1){
	  printf("Erreur de création mémoire p\n");
	  exit(1);
	}
	/*Attachemment de la mémoire partagé*/
	mp = shmat(memp,0,0);
	mp[0]=0;/*ndp*/
	mp[1]=0;/*npo*/
	
	shmdt(mp);
	/*pp*/
	for(int j=0;j<10;j++){
	  p=fork();
	  if(p==-1){
	    printf("Erreur de création de processus\n");
	  }
	  if(p==0){
	    char str[10];
	    sprintf(str,"%d",j);
	    err = execlp("./Pgme_nageur","Pgme_nageur",str,NULL);
	    printf("Erreur d'éxécution de programme de client\n");
	    exit(1);
	  }
	}
	while(wait(NULL)!=-1){
	  printf("fils terminé\n");
	}
	
	semctl(sem,IPC_RMID,0);
	shmctl(memp,IPC_RMID,0);
	printf("Fin du processus\n");
	
	
	
  return 0;
}
