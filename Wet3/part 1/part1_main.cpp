// C program to demonstrate working of Semaphores 
#include <stdio.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include "Semaphore.hpp"



Semaphore* sem;
void* thread(void* arg) 
{ 
	//wait 
	sem->down(); 
	printf("\nEntered..\n"); 

	//critical section 
	sleep(1); 
	
	//signal 
	printf("\nJust Exiting...\n"); 
	sem->up(); 
	return NULL;
} 


int main() 
{ 
	sem = new Semaphore(1);
	// sem_init(&sem, 0, 1); 
	pthread_t t1,t2,t3,t4; 
	pthread_create(&t1,NULL,thread,NULL); 
	// sleep(2); 
	pthread_create(&t2,NULL,thread,NULL); 
	// sleep(2); 
	pthread_create(&t3,NULL,thread,NULL); 
	// sleep(2); 
	pthread_create(&t4,NULL,thread,NULL); 
	// sleep(2); 
	pthread_join(t1,NULL); 
	pthread_join(t2,NULL); 
	pthread_join(t3,NULL); 
	pthread_join(t4,NULL); 
	delete sem;
	
	sem = new Semaphore();
	sem->up();
	sem->down();
	delete sem;
	return 0; 
} 
