#include <cstdlib.h>
#include <unistd.h>
#include <list>
#include <iterator> 
#include "AllocationData.hpp"

#define MIN_SIZE 0
#define MAX_SIZE 100000000

list<AllocationData> allocationsHistory ;

void* malloc(size_t size) {
	if (size <= MIN_SIZE || size > MAX_SIZE)
		return NULL;
  
	/*Search for enough space in released structures*/
	
	list<AllocationData>::iterator it; 
    for(it = allocationsHistory.begin(); it != allocationsHistory.end(); ++it){
		if(it.is_free() && it.get_requested_size()>=size){
			it.set_is_free(false);
			return it.get_effective_allocation();
		}	
	}
  
	/*Not enough space was found in the released structers*/
	
	//allocate for meta_data
	AllocationData *meta_data = (AllocationData *)sbrk(sizeof(AllocationData));
	if (meta_data == (void*)(-1))
		return NULL;
  
	//allocate for effective_allocation
	void *effective_allocation = sbrk(size);
	if (effective_allocation == (void*)(-1))
		return NULL;
	
	meta_data->set_is_free(false);
	meta_data->set_requested_size(size);
	meta_data->set_effective_allocation(effective_allocation);	
	allocationsHistory.add(meta_data);
	
	return effective_allocation;
}

void* calloc(size_t size) {

}

void free(void* p) {

}

void* realloc(void* oldp, size_t size) {

}
