#include "Semaphore.hpp"

Semaphore::Semaphore() {
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c, NULL);
	counter = 0;
}

Semaphore::Semaphore(unsigned val) {
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c, NULL);
	counter = val;
}

Semaphore::~Semaphore() {
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&c);
}

void Semaphore::up() {
	pthread_mutex_lock(&m);
	//Pull and wake up the first thread in the waiting queue
	pthread_cond_signal(&c);
	counter++;
	pthread_mutex_unlock(&m);
}

void Semaphore::down() {
	pthread_mutex_lock(&m);
	while (counter == 0) {
		//Send the thread to the waiting queue 
		pthread_cond_wait(&c, &m);
	}
	counter--;
	pthread_mutex_unlock(&m);
}