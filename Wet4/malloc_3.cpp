#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "AllocationData.hpp"

#include <iostream>

#define MIN_SIZE 0
#define MIN_SPLIT_SIZE 128
#define MAX_SIZE 100000000

/* =========================== Declarations =========================== */

size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();

size_t alignment(size_t size);
void split(AllocationData* metaData, size_t new_requested_size);
void combine(AllocationData* metaData);

/* ========================= Declarations End ========================= */

/* ================================== Helper Function ================================== */

size_t alignment(size_t size) {
	return (((size - 1) / 4) + 1) * 4;
}

void split(AllocationData* metaData, size_t new_requested_size) {
	size_t alignedSizeofAllocationData = _size_meta_data();
	size_t alignedSize = alignment(new_requested_size);
	int size = metaData->get_original_size() - alignedSize - alignedSizeofAllocationData;

	if (size < MIN_SPLIT_SIZE) {
		return;
	}

	AllocationData* newMatadata = (AllocationData*)((size_t)metaData
												+ alignedSizeofAllocationData + alignedSize);
	newMatadata->set_is_free(true);
	newMatadata->set_original_size(size);
	newMatadata->set_allocation_addr((void*)((size_t)newMatadata
														+ alignedSizeofAllocationData));
	newMatadata->set_next(metaData->get_next());
	newMatadata->set_prev(metaData);

	metaData->set_next(newMatadata);
	metaData->set_original_size(alignedSize);

	if (newMatadata->get_next()) {
		(newMatadata->get_next())->set_prev(newMatadata);
	}

	combine(newMatadata);
}

void combine(AllocationData* metaData) {
	AllocationData* adjacent_block;
	size_t alignedSizeofAllocationData = alignment(sizeof(AllocationData));

	// First, we check if the upper adjacent block is free. In case it is, we combine both
	adjacent_block = metaData->get_next();
	// If the allocated block is the last one on the list, there is no need for combination
	if (adjacent_block && adjacent_block->is_free()) {
		metaData->set_original_size(metaData->get_original_size()
											+ adjacent_block->get_original_size()
											+ alignedSizeofAllocationData);
		metaData->set_next(adjacent_block->get_next());

		if (adjacent_block->get_next()) {
			(adjacent_block->get_next())->set_prev(metaData);
		}
	}

	// Now, we check if the lower adjacent block is free. In case it is, we combine both
	adjacent_block = metaData->get_prev();
	if (adjacent_block && adjacent_block->is_free()) {
		adjacent_block->set_original_size(metaData->get_original_size()
													+ adjacent_block->get_original_size()
													+ alignedSizeofAllocationData);
		adjacent_block->set_next(metaData->get_next());

		if (metaData->get_next()) {
			(metaData->get_next())->set_prev(adjacent_block);
		}
	}
}


/* ================================== Helper Function End ================================== */

AllocationData* allocHistory = NULL;

void* malloc(size_t size) {
   if (size <= MIN_SIZE || size > MAX_SIZE) {
      return NULL;
   }

   size_t alignedSize = alignment(size);
	bool wildernessFlag = false;
   AllocationData* metaData = NULL;
   AllocationData* it = NULL;

   // First, we search for freed space in our global list
   if (allocHistory) {
      for (it = allocHistory; it; it = it->get_next()) {
			// Checking if there is a big enough *free* space
         if (it->get_original_size() >= size && it->is_free()) {
            metaData = it;
            break;
         }
			// Checking for wilderness option
			if (!(it->get_next()) && it->is_free() && it->get_original_size() < size) {
				metaData = it;
				void* allocAddition = sbrk(alignedSize - it->get_original_size());
				if (allocAddition == (void*)(-1)) {
					sbrk(it->get_original_size() - alignedSize);
					return NULL;
				}
				wildernessFlag = true;
			}
      }

      if (metaData) {
         metaData->set_is_free(false);
         metaData->set_requested_size(size);
			if (wildernessFlag) {
				metaData->set_original_size(alignedSize);
			}
			if (metaData->get_original_size() > size || wildernessFlag) {
				split(metaData, size);
			}
         return metaData->get_allocation_addr();
      }
   }

   // Not enough freed space was found, so we allocate new space
   // Allocating Metadata
   metaData = (AllocationData*)sbrk(alignment(sizeof(AllocationData)));
   if (metaData == (void*)(-1)) {
      return NULL;
   }

   // Allocating requested_size bytes
   void* allocation_addr = sbrk(alignedSize);
   if (allocation_addr == (void*)(-1)) {
      sbrk(-sizeof(AllocationData));
	return NULL;
   }

   // Setting up the meta data
   metaData->set_is_free(false);
   metaData->set_original_size(alignedSize);
   metaData->set_requested_size(size);
   metaData->set_allocation_addr(allocation_addr);
   metaData->set_next(NULL);
   metaData->set_prev(NULL);

   // Adding the allocation meta-data to the allocation history list
   // For the first allocation
   if (!allocHistory) {
      allocHistory = metaData;
   }
   else {
      // In case there are others
      it = allocHistory;
      while (it->get_next()) {
         it = it->get_next();
      }
      metaData->set_prev(it);
      it->set_next(metaData);
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

   // If oldp is NULL, we allocate space for 'size' bytes and return a pointer to it
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
   AllocationData* metaData = NULL;
   for (AllocationData* it = allocHistory; it; it = it->get_next()) {
      if (it->get_allocation_addr() == oldp) {
         metaData = it;
         break;
      }
   }

   // We check if the previously made allocation is big enough to facilitate the new block size
   if (metaData->get_original_size() >= size) {
      metaData->set_requested_size(size);
		if (metaData->get_original_size() > size) {
			split(metaData, size);
		}
      return oldp;
   }
   else {
		// We check for wilderness option
		if (!(metaData->get_next())) {
			size_t alignedSize = alignment(size);
			void* allocAddition = sbrk(alignedSize - metaData->get_original_size());
			if (allocAddition == (void*)(-1)) {
				sbrk(metaData->get_original_size() - alignedSize);
				return NULL;
			}
			metaData->set_requested_size(size);
			metaData->set_original_size(alignedSize);
			return metaData->get_allocation_addr();
		}

		// We check if adjacent block is free and big enough to facilitate the new allocation
		if (metaData->get_next() && metaData->get_next()->is_free()) {
			size_t combinedSize = metaData->get_original_size()
											+ _size_meta_data() + metaData->get_next()->get_original_size();
			if (combinedSize >= size) {
				combine(metaData);
				split(metaData, size);
				return metaData->get_allocation_addr();
			}
		}

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
   return alignment(sizeof(AllocationData));
}

size_t _num_meta_data_bytes() {
   size_t num_meta_data_blocks = _num_allocated_blocks();
   return num_meta_data_blocks * _size_meta_data();
}
