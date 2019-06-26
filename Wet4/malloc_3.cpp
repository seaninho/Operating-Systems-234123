#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "AllocationData.hpp"

#include <iostream>

#define MIN_SIZE 0
#define MAX_SIZE 100000000


/* ================================== Helper Function ================================== */

size_t alignment(size_t size) {
	while (size != ((int)(size/4))*4) {
		size++;
	}
	return size;
}

void split(AllocationData* meta_data, size_t new_requested_size ) {
	size_t aligned_size_AllocationData = alignment(sizeof(AllocationData));
	size_t aligned_size = alignment(new_requested_size);
	int size = meta_data->get_original_size() - aligned_size - aligned_size_AllocationData;

	if (size < 128) {
		return;
	}

	AllocationData* new_meta_data = (AllocationData*)(meta_data + aligned_size_AllocationData + aligned_size);
	new_meta_data->set_is_free(true);
	new_meta_data->set_original_size(alignment(size));
	new_meta_data->set_allocation_addr((void*)(new_meta_data + aligned_size_AllocationData) );
	new_meta_data->set_next(meta_data->get_next());
	new_meta_data->set_prev(meta_data);

	meta_data->set_next(new_meta_data);
	meta_data->set_original_size(aligned_size);

	if (new_meta_data->get_next()) {
		(new_meta_data->get_next())->set_prev(new_meta_data);
	}

}

void combine(AllocationData* meta_data) {
	AllocationData* adjacent_block;
	size_t aligned_size_AllocationData = alignment(sizeof(AllocationData));

	// First, we check if the upper adjacent block is free. In case it is, we combine both
	adjacent_block = meta_data->get_next();
	// If the allocated block is the last one on the list, there is no need for combination
	if (adjacent_block && adjacent_block->is_free()) {
		meta_data->set_original_size(meta_data->get_original_size()
											+ adjacent_block->get_original_size()
											+ aligned_size_AllocationData);
		meta_data->set_next(adjacent_block->get_next());

		if (adjacent_block->get_next()) {
			(adjacent_block->get_next())->set_prev(meta_data);
		}
		return;
	}

	// Now, we check if the lower adjacent block is free. In case it is, we combine both
	adjacent_block = meta_data->get_prev();
	if (adjacent_block && adjacent_block->is_free()) {
		adjacent_block->set_original_size(meta_data->get_original_size()
													+ adjacent_block->get_original_size()
													+ aligned_size_AllocationData);
		adjacent_block->set_next(meta_data->get_next());

		if (meta_data->get_next()) {
			(meta_data->get_next())->set_prev(adjacent_block);
		}
	}
}


/* ================================== Heleper Function End ================================== */

AllocationData* allocHistory = NULL;

void* malloc(size_t size) {
   if (size <= MIN_SIZE || size > MAX_SIZE) {
      return NULL;
   }

   size_t aligned_size = alignment(size);

   AllocationData* meta_data = NULL;
   AllocationData* it = allocHistory;

   // First, we search for freed space in our global list
   if (allocHistory) {
      for (; it; it = it->get_next()) {
         if (it->is_free() && it->get_original_size() >= size) {
            meta_data = it;
            break;
         }
      }

      if (meta_data) {
         meta_data->set_is_free(false);
         meta_data->set_requested_size(size);
		 	split(meta_data,size);
         return meta_data->get_allocation_addr();
      }
   }

   //handle problem 3
	if (allocHistory) {
		for (it = allocHistory; it; it = it->get_next()) {
			//if 'it' is not last and free we do nothing
			if (it->get_next() && it->is_free()) {
				break;
			}

			//the wilderness chenk is free and not big enough (otherwise we would handle it in the previous for loop)
			if (!(it->get_next()) && it->is_free()) {

				void* add_allocation = sbrk(aligned_size-it->get_original_size());
				if (add_allocation == (void*)(-1)) {
					brk((void*) it);
					return NULL;
				}

				// Setting up the meta data
				meta_data = it;
				meta_data->set_is_free(false);
				meta_data->set_original_size(aligned_size);
				meta_data->set_requested_size(size);
				return meta_data->get_allocation_addr();
			}
		}
	}

   // Not enough freed space was found, so we allocate new space
   // Allocating meta_data
   meta_data = (AllocationData*)sbrk(alignment(sizeof(AllocationData)));
   if (meta_data == (void*)(-1)) {
      return NULL;
   }

   // Allocating requested_size bytes
   void* allocation_addr = sbrk(aligned_size);
   if (allocation_addr == (void*)(-1)) {
      sbrk(-sizeof(AllocationData));
	return NULL;
   }

   // Setting up the meta data
   meta_data->set_is_free(false);
   meta_data->set_original_size(aligned_size);
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
      it = allocHistory;
      while (it->get_next()) {
         it = it->get_next();
      }
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
   for (AllocationData* it = allocHistory; it; it = it->get_next()) {
      if (it->get_allocation_addr() == p) {
         // If 'p' was already released, we ignore the action
      	if (it->is_free()) {
            return;
         }
         // If not, we free the allocated block
         else {
            it->set_is_free(true);
				combine(it);
            return;
         }
      }
	}

}

void* realloc(void* oldp, size_t size) {
   if (size <= MIN_SIZE || size > MAX_SIZE) {
      return NULL;
   }

   // If oldp is NULL, we allocate space for 'size' bytes and return a poiter to it
   if (!oldp) {
      void* allocation_addr = malloc(size);
      if (!allocation_addr) {
         return NULL;
      }
      else {
         return allocation_addr;
      }
   }

   // If not, we search for it assuming oldp is a pointer to a previously allocated block
   AllocationData* meta_data = NULL;
   for (AllocationData* it = allocHistory; it; it = it->get_next()) {
      if (it->get_allocation_addr() == oldp) {
         meta_data = it;
         break;
      }
   }

   // We determine whether allocation has enough space to facilitate the new block size
   if (size <= meta_data->get_original_size()) {
      meta_data->set_requested_size(size);
      return oldp;
   }
   else {
      // If not, we allocate a new space (possibly reused allocation)
      void* allocation_addr = malloc(size);
      if (!allocation_addr) {
         return NULL;
      }
      // We copy the data and then free the old space
      std::memcpy(allocation_addr, oldp, size);
      free(oldp);
      return allocation_addr;
   }

}

size_t _num_free_blocks() {
   size_t num = 0;
   size_t num_of_elem = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; it; it = it->get_next()) {
         num_of_elem++;
         if (it->is_free()) {
            num++;
         }
      }
   }
   return num;
}

size_t _num_free_bytes() {
   size_t num = 0;
   size_t num_of_elem = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; it; it = it->get_next()) {
         num_of_elem++;
         if (it->is_free()) {
            num += it->get_original_size();
         }
      }
   }
   return num;
}

size_t _num_allocated_blocks() {
   size_t num = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; it; it = it->get_next()) {
         num++;
      }
   }
   return num;
}

size_t _num_allocated_bytes() {
   size_t num = 0;
   if (allocHistory) {
      for (AllocationData* it = allocHistory; it; it = it->get_next()) {
         num += it->get_original_size();
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
