#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h> 
#include <semaphore.h>
#include <pthread.h>

#define CHAIRS 2
#define CUSTOMERS 5

sem_t *waitingRoom;
sem_t *barberSeat;
sem_t *doneWC;
sem_t *barberSleep;

int done = 0;

void initialize_semaphores(){
    /* Initialize the waiting room */
    waitingRoom = sem_open("/waitingRoom",O_CREAT,0644,CHAIRS);
    /* Initialize the barber seat */
    barberSeat = sem_open("/barberSeat",O_CREAT,0644,1);
    /* Initialize done with customers */
    doneWC = sem_open("/doneWC",O_CREAT,0644,0);
    /* Initialize barber sleep */
    barberSleep = sem_open("/barberSleep",O_CREAT,0644,0);
    /* Check if any of these failed */
    if (waitingRoom == SEM_FAILED || barberSeat == SEM_FAILED || doneWC == SEM_FAILED || barberSleep == SEM_FAILED) {
        perror("Failed to open all semaphores, shutting down...");
        exit(-1);
    }
}

void unlink_semaphores(){
    if (sem_unlink("/doneWC") != 0 || sem_unlink("/barberSleep") !=0 || sem_unlink("/waitingRoom") !=0 || sem_unlink("/barberSeat")) {
        perror("Error unlinking semaphores");
        exit(-1);
    }
}

void *customer(void *args){
    int number = *(int *) args;
    printf("Customer %i> Wants to cut his hair...\n",number);
    sleep(rand()%5 + 1);
    printf("Customer %i> Just got to the barber shop!\n",number);
    sem_wait(waitingRoom);
    printf("Customer %i> Just entered the waiting room\n",number);
    /* He's gonna now wait for the barber */
    sem_wait(barberSeat); 
    sem_post(waitingRoom);
    sem_post(barberSleep); 
    sem_wait(doneWC);
    sem_post(barberSeat);
    printf("Customer %i> Just had his haircut, thx barber!..\n",number);
}

void *barber(void *args){
    while(!done){
        printf("Barber is sleeping...\n");
        sem_wait(barberSleep);
        if(!done){
            printf("Barber is cutting hair..\n");
            sleep(rand()%3 + 1);
            printf("Barber finished cutting hair, sleep time~!\n");
            sem_post(doneWC);
        }else{
            printf("Barber is done, closing...");
        }
    }
}

int main(void){
    initialize_semaphores();
    /* Receive the ID for the barber and customer threads */
    pthread_t bbrtid;
    pthread_t cmrid[CUSTOMERS];
    /* Create barber thread */
    pthread_create(&bbrtid,NULL,barber,NULL);

    int Number[CUSTOMERS];
    
    /* Create customer threads */
    for(int i = 0; i < CUSTOMERS; i++){
        Number[i] = i;
        pthread_create(&cmrid[i],NULL,customer,(void *)&Number[i]);
    }
    /* Join all threads */
    for (int i = 0; i < CUSTOMERS; i++){
        pthread_join(cmrid[i],0);
    }
    done = 1;
    sem_post(barberSleep);
    pthread_join(bbrtid,NULL);
    unlink_semaphores();
    return 0;
}
