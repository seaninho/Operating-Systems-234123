class AllocationData {
public:

	void set_is_free(bool free) {
		_is_free = free;
	}

	void set_requested_size(size_t size) {
		_requested_size = size;
	}

	void set_allocation_addr(void *addr) {
		_allocation_addr = addr;
	}

   void set_next(AllocationData* next) {
      _next = next;
   }

   void set_prev(AllocationData* prev) {
      _prev = prev;
   }

	bool is_free() {
		return _is_free;
	}

	size_t get_requested_size() {
		return _requested_size;
	}

	void* get_allocation_addr() {
		return _allocation_addr;
	}

   AllocationData* get_next() {
      return _next;
   }

   AllocationData* get_prev() {
      return _prev;
   }

private:
	bool _is_free;
	size_t _requested_size;
	void* _allocation_addr;
   AllocationData* _next;
   AllocationData* _prev;
};
