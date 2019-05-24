#include "PCQueue.hpp"

template <class T> 
PCQueue<T>::PCQueue(){
	pthread_mutex_init(&m, NULL);
	sem = new Semaphore(0);
}

template <class T>
PCQueue<T>::~PCQueue() {
	pthread_mutex_destroy(&m);
	delete sem;
}

template <class T>
T PCQueue<T>::pop(){
	sem.down();
	pthread_mutex_lock(&m);
	T temp = queue.pop();
	pthread_mutex_unlock(&m);
	return temp;
}

template <class T>
void PCQueue<T>::push(const T& item){
	pthread_mutex_lock(&m);
	queue.push(item);
	pthread_mutex_unlock(&m);
	sem.up();
}
