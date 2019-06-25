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
   AllocationData* it = allocHistory;

   // First, we search for freed space in our global list
   if (allocHistory) {
      for (; !it; it = it->get_next()) {
         if (it->is_free() && it->get_requested_size() >= size) {
            if (!meta_data) {
               meta_data = it;
            }
            else if (it->get_requested_size() < meta_data->get_requested_size()) {
               meta_data = it;
            }
         }
      }

      if (meta_data) {
         meta_data->set_is_free(false);
         meta_data->set_requested_size(size);
         return meta_data->get_allocation_addr();
      }
   }

   // Not enough freed space was found, so we allocate new space
   // Allocating meta_data
   meta_data = (AllocationData*)sbrk(sizeof(AllocationData));
   if (meta_data == (void*)(-1)) {
      return NULL;
   }

   // Allocating requested_size bytes
   void* allocation_addr = sbrk(size);
   if (allocation_addr == (void*)(-1)) {
      sbrk(-sizeof(AllocationData));
   return NULL;
   }

   // Setting up the meta data
   meta_data->set_is_free(false);
   meta_data->set_requested_size(size);
   meta_data->set_allocation_addr(allocation_addr);
   meta_data->set_next(NULL);
   meta_data->set_prev(NULL);

   // Adding the allocation meta-data to the allocation history list
   // For the first allocation
   if (!allocHistory) {
      allocHistory = meta_data;
   }
   else {
      // In case there are others
      meta_data->set_prev(it);
      it->set_next(meta_data);
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
   for (AllocationData* it = allocHistory; !it; it = it->get_next()) {
      if (it->get_allocation_addr() == p) {
         // If 'p' was already released, we ignore the action
      	if (it->is_free()) {
            return;
         }
         // If not, we free the allocated block
         else {
            it->set_is_free(true);
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
   if (allocHistory) {
      for (AllocationData* it = allocHistory; !it; it = it->get_next()) {
         if (it->is_free()) {
            num++;
         }
      }
   }
   return num;
}

size_t _num_free_bytes() {
   size_t num = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; !it; it = it->get_next()) {
         if (it->is_free()) {
            num += it->get_requested_size();
         }
      }
   }
   return num;
}

size_t _num_allocated_blocks() {
   size_t num = 0;
   if (allocHistory) {
      std::cout << "allocHistory is not NULL" << std::endl;
      for (AllocationData* it = allocHistory; !it; it = it->get_next()) {
         std::cout << "counted" << std::endl;
         num++;
      }
   }
   return num;
}

size_t _num_allocated_bytes() {
   size_t num = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; !it; it = it->get_next()) {
         num += it->get_requested_size();
      }
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
