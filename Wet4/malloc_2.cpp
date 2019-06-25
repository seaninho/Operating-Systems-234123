#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "AllocationData.hpp"

#include <iostream>

#define MIN_SIZE 0
#define MAX_SIZE 100000000

AllocationData* allocHistory = NULL;

void* malloc(size_t size) {
   if (size <= MIN_SIZE || size > MAX_SIZE)
   return NULL;

   AllocationData* meta_data = NULL;
   std::cout << "1" << std::endl;
   // First, we search for freed space in our global list
   while (!allocHistory) {
      if (allocHistory->is_free() && allocHistory->get_requested_size() >= size) {
         if (!meta_data) {
            meta_data = allocHistory;
         }
         else if (allocHistory->get_requested_size() < meta_data->get_requested_size()) {
            meta_data = allocHistory;
         }
      }
      allocHistory = allocHistory->get_next();
   }
   std::cout << "2" << std::endl;
   if (meta_data) {
      meta_data->set_is_free(false);
      meta_data->set_requested_size(size);
      return meta_data->get_allocation_addr();
   }
   std::cout << "3" << std::endl;
   // Not enough freed space was found, so we allocate new space
   // Allocating meta_data
   meta_data = (AllocationData*)sbrk(sizeof(AllocationData));
   if (meta_data == (void*)(-1)) {
      return NULL;
   }
   std::cout << "4" << std::endl;
   // Allocating requested_size bytes
   void* allocation_addr = sbrk(size);
   if (allocation_addr == (void*)(-1)) {
      sbrk(-sizeof(AllocationData));
   return NULL;
   }
   std::cout << "5" << std::endl;
   // Setting up the meta data
   meta_data->set_is_free(false);
   meta_data->set_requested_size(size);
   meta_data->set_allocation_addr(allocation_addr);
   meta_data->set_next(NULL);
   meta_data->set_prev(NULL);
   std::cout << "6" << std::endl;
   // Adding the allocation meta-data to the allocation history list
   // For the first allocation
   if (!allocHistory) {
      allocHistory = meta_data;
   }
   else {
      // In case there are others
      meta_data->set_prev(allocHistory);
      allocHistory->set_next(meta_data);
   }
   
   return allocation_addr;
   }


void* calloc(size_t num, size_t size) {
   // First, we allocate a new space (possibly reused allocation)
   void* allocation_addr = malloc(num * size);
   if (!allocation_addr) {
      return NULL;
   }
   else {
      // If we succeed in allocating, we clear the block
      return std::memset(allocation_addr, 0, num * size);
   }
}

void free(void* p) {
   if (!p) {
      return;
   }

   // We search for p in our global list
   while (!allocHistory) {
      if (allocHistory->get_allocation_addr() == p) {
         // If 'p' was already released, we ignore the action
      	if (allocHistory->is_free()) {
            return;
         }
         // If not, we free the allocated block
         else {
            allocHistory->set_is_free(true);
            return;
         }
      }
	}

}

void* realloc(void* oldp, size_t size) {
   // First, we allocate a new space (possibly reused allocation)
   void* allocation_addr = malloc(size);
   if (!allocation_addr) {
      return NULL;
   }

   // Then, we check if the old address is NULL
   // If so, we don't copy and then free the old data
   if (!oldp) {
      return allocation_addr;
   }
   else {
      // We copy the data and then free the old space
      std::memcpy(allocation_addr, oldp, size);
      free(oldp);
      return allocation_addr;
   }

}

size_t _num_free_blocks() {
   size_t num = 0;
   while (!allocHistory) {
      if (allocHistory->is_free()) {
         num++;
      }
      allocHistory = allocHistory->get_next();
   }
   return num;
}

size_t _num_free_bytes() {
   size_t num = 0;
   while (!allocHistory) {
      if (allocHistory->is_free()) {
         num += allocHistory->get_requested_size();
      }
      allocHistory = allocHistory->get_next();
   }
   return num;
}

size_t _num_allocated_blocks() {
   size_t num = 0;
   while (!allocHistory) {
      num++;
      allocHistory = allocHistory->get_next();
   }
   return num;
}

size_t _num_allocated_bytes() {
   size_t num = 0;
   while (!allocHistory) {
      num += allocHistory->get_requested_size();
      allocHistory = allocHistory->get_next();
   }
   return num;
}

size_t _size_meta_data() {
   return sizeof(AllocationData);
}

size_t _num_meta_data_bytes() {
   size_t num_meta_data_blocks = _num_allocated_blocks();
   return num_meta_data_blocks * _size_meta_data();
}
