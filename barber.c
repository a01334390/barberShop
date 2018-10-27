#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define CHAIRS 3
#define CUSTOMERS 10

sem_t waitingRoom;
sem_t barberSeat;
sem_t doneWc;
sem_t barberSleep;

int done = 0;

void initialize_semaphores(){
    if(sem_init(&waitingRoom,0,CHAIRS) == -1){
        perror("ERROR! Couldn't create waiting room. Shutting down...");
        exit(-1);
    }

    if(sem_init(&barberSeat,0,1) == -1){
        perror("ERROR! Couldn't create barber seat. Shutting down...");
        exit(-1);
    }

    if(sem_init(&doneWc,0,0) == -1){
        perror("ERROR! Couldn't create DoneWC. Shutting down...");
        exit(-1);
    }

    if(sem_init(&barberSleep,0,0) == -1){
        perror("ERROR! Couldn't create Barber sleep. Shutting down...");
        exit(-1);
    }
}

void *customer(void *custNumber){
    int number = *(int *) custNumber;
    printf("Customer %i> Wants to cut his hair...\n",number);
    sleep(1);
    printf("Customr %i> Just got to the barber shop!\n",number);
    sem_wait(&waitingRoom);
    printf("Customer %i> Just entered the waiting room\n",number);
    /* He's gonna now wait for the barber */
    sem_wait(&barberSeat); 
    sem_post(&waitingRoom); 
    sem_post(&barberSleep); 
    sem_wait(&done);
    sem_post(&barberSeat);
    printf("Customer %i> Just cut his hair, thx barber!\n",number);
}

void *barber(void *args){
    while(!done){
        printf("Barber is sleeping...\n");
        sem_wait(&barberSleep);
        if(!done){
            printf("Barber is cutting hair..\n");
            sleep(1);
            sem_post(&done);
        }else{
            printf("Barber is done, closing...");
        }
    }
}

int main(){
    /* Initialize semaphores */
    initialize_semaphores();
    /* Receive the ID for the barber and customer threads */
    pthread_t bbrtid;
    pthread_t cmrid[CUSTOMERS];
    /* Create barber thread */
    pthread_create(&bbrtid,0,barber,0);
    /* Create customer threads */
    int customerID[CUSTOMERS];
    for(int i = 0; i < CUSTOMERS; i++){
        customerID[i] = i;
        pthread_create(&cmrid[i],0,customer,&customerID[i]);
    }
    /* Join all threads */
    for (int i = 0; i < CUSTOMERS; i++){
        pthread_join(cmrid[i],0);
    }
    done = 1;
    sem_post(&barberSleep);
    pthread_join(bbrtid,0);
}
