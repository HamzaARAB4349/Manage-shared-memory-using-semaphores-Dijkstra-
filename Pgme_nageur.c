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

#define np 5
#define nc 2

#define scab 0
#define mutex 1
#define span 2

struct sembuf pscab = {scab,-1,0};
struct sembuf vscab = {scab,1,0};
struct sembuf pmutex = {mutex,-1,0};
struct sembuf vmutex = {mutex,1,0};
struct sembuf pspan = {span,-1,0};
struct sembuf vspan = {span,1,0};

int sem,memp;
int *mp;
key_t clef;

void Demander_panier(int *nump){
  semop(sem,&pmutex,1);
  mp[0] = mp[0] +1;
  if(mp[1] == np){
    semop(sem,&vmutex,1);
    semop(sem,&pspan,1);
  }
  mp[0] = mp[0] -1;
  mp[1] = mp[1] +1;
  *nump = mp[1];
  semop(sem,&vmutex,1);
}

void Demander_cabine(){
  semop(sem,&pscab,1);
}

void Liberer_cabine(){
  semop(sem,&vscab,1);
}

void Liberer_panier(int i, int *nump){
  semop(sem,&pmutex,1);
  mp[1] = mp[1] -1;
  *nump = mp[1];
  if(mp[0]>0){
    printf("Nageur %d va liberer un panier et il ya : %d demandes en attente \n" , i ,mp[0]);
    semop(sem,&vspan,1);
  }else{
    semop(sem,&vmutex,1);
  }
}

int main(int argc,char *argv[]){
         
         int i;
         if (argc > 1)
         {
            i = atoi(argv[1]);
          }
          else
         {
            printf("Pas de parametre! \n");
         }
         
       /*Récupération de sémaphores*/
        clef = ftok("main.c",2); 
	sem = semget(clef,3,IPC_CREAT | 0666);
	if(sem == -1){
	  printf("Erreur de Récupération semaphores \n");
	  exit(1);
	}  
	
	/*Récupération de la mémoire partagée*/
	clef = ftok("main.c",3);
	memp = shmget(clef,2*sizeof(int),IPC_CREAT | 0666);
	if(memp == -1){
	  printf("Erreur de Récupération mémoire p\n");
	  exit(1);
	}
	/*Attachemment de la mémoire partagé*/
	mp = shmat(memp,0,0);
	
	/*pp*/
	int nump;
	
	Demander_panier(&nump);
	Demander_cabine();
	printf("Nageur %d se change et range ses vetments\n",i);
	sleep(4);
	Liberer_cabine();
	printf("Nageur %d se baigner\n",i);
	sleep(7);
	printf("Je suis le nageur numéro =%d j'occupe le panier %d \n",i,nump);
	Demander_cabine();
	printf("Nageur %d se change et range ses vetments\n",i);
	sleep(4);
	Liberer_cabine();
	Liberer_panier(i,&nump);
	printf("Je suis le nageur : %d j'ai liberer un panier ,il reste :%d  panier libres\n",i,np-nump);
        shmdt(mp);	

  return 0;
}
