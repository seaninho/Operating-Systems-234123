class AllocationData {
public:
	AllocationData(uint size) {
		_is_free = false;
		_requested_size = size;
		_effective_allocation = NULL;
	}

	void set_is_free(bool free){
		_is_free = free;
	}
	void set_requested_size(uint size){
		_requested_size = size;
	}
	void set_effective_allocation(void *addr){
		_effective_allocation = addr;
	}

	bool is_free(){
		return _is_free;
	}
	uint get_requested_size(){
		return _requested_size;
	}
	void* get_effective_allocation(){
		return _effective_allocation;
	}
	
private:
   bool _is_free;
   uint _requested_size;
   void *_effective_allocation;
}
