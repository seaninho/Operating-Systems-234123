class AllocationData {
public:
	AllocationData(size_t size) {
		_is_free = false;
		_requested_size = size;
		_allocation_addr = NULL;
	}

	void set_is_free(bool free) {
		_is_free = free;
	}

	void set_requested_size(size_t size) {
		_requested_size = size;
	}

	void set_allocation_addr(void *addr) {
		_allocation_addr = addr;
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

private:
	bool _is_free;
	size_t _requested_size;
	void* _allocation_addr;
}
