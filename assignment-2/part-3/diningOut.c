#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "diningOut.h"

#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define DONE 3
#define LEFT (n + 4) % 5
#define RIGHT (n + 1) % 5

int philosophers[5];
int state[5];

sem_t m; // binary semaphore for critical code in take or put fork
sem_t semaphore[5]; // semaphores for locking down the forks/chopsticks

int main() {
    // Initialize philosophers
    for(int i=0; i<5; i++) {
        philosophers[i] = i;
    }

    // Initialize semaphores
    sem_init(&m, 0, 1);

    for(int i=0; i<5; i++) {
        sem_init(&semaphore[i], 0, 0);
    }

    // Create pthreads
    pthread_t threads[5];

    for(int i=0; i<5; i++) {
        pthread_create(&threads[i], NULL, philosopher, &philosophers[i]);
    }

    // Wait for threads to exit
    for(int i=0; i<5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *philosopher(void *n) {
    int *num = n;

    printf("Philosopher %d is thinking\n", *num+1);

    for(;;) {
        // Philosopher is thinking (wait)
        sleep(1);

        take_fork(*num); // pick up fork if available

        // if philosopher is done eating, exit thread
        if(state[*num] == DONE) {
            break;
        }

        put_fork(*num);
    }

    pthread_exit(NULL);
}

void take_fork(int n) {
    sem_wait(&m //Lockdown critical section

    state[n] = HUNGRY;
    printf("Philosopher %d is hungry\n", n+1);

    eat(n);

    sem_post(&m); //Unlock critical section

    sem_wait(&semaphore[n]); //Lock fork

    sleep(1);
}

void put_fork(int n) {
    sem_wait(&m); //Lock critical section
	
    state[n] = THINKING;
    printf("Philosopher %d is thinking\n", n+1);

    eat(LEFT); // try to eat if left
    eat(RIGHT); // and right forks available

    sem_post(&m); // Unlock critical section
}

void eat(int n) {
    if(state[n] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[n] = EATING;

        sleep(2);

        printf("Philosopher %d is eating\n", n+1);

        sem_post(&semaphore[n]); //Unlock fork after eating

        state[n] = DONE;
        printf("Philosopher %d is done\n", n+1);
    }
}