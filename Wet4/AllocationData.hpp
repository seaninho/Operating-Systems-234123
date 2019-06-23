class AllocationData {
public:
	AllocationData(size_t size) {
		_is_free = false;
		_requested_size = size;
		_effective_allocation = NULL;
	}

	void set_is_free(bool free){
		_is_free = free;
	}
	void set_requested_size(size_t size){
		_requested_size = size;
	}
	void set_effective_allocation(void *addr){
		_effective_allocation = addr;
	}

	bool is_free(){
		return _is_free;
	}
	size_t get_requested_size(){
		return _requested_size;
	}
	void* get_effective_allocation(){
		return _effective_allocation;
	}
	
private:
	bool _is_free;
	size_t _requested_size;
	void *_effective_allocation;
}
