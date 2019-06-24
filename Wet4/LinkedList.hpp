
template <class T>
class LinkedList {
	
public:
	class Node {
	public:
	   T data;
	   Node* next;
	};
	
	class iterator {
		Node* ptr_node ;

	public:
		iterator( Node* ptr_node) : ptr_node(ptr_node) {}

		iterator& operator=(const iterator& iterator ){
			if( ptr_node == NULL )  {
				return *this ;
			}
			ptr_node = iterator.ptr_node ;
			return *this ;
		}
		
		bool operator==( const iterator& iterator ){
			if( ptr_node == NULL && iterator.ptr_node == NULL )  {
				return true ;
			}
			if( ptr_node == NULL || iterator.ptr_node == NULL )  {
				return false ;
			}
			return ptr_node == iterator.ptr_node ;
		}
		
		bool operator!=( const iterator& iterator ){
			return !( *this == iterator );
		}
			
		iterator& operator++(){
			if( ptr_node == NULL ) {
				return *this ;
			}
			ptr_node = ptr_node->next ;
			return *this ;
		}
		
		const T& operator*(){
			return ptr_node->data ;
		}


    };
	

	Node* first;
	Node* last;
	
	LinkedList<T>(){
		first = NULL;
		last = NULL;
	}

	void add(T data) {
		if(!first) {
			// The list is empty
			first = (Node*)sbrk(sizeof(Node));
			first->data = data;
			first->next = NULL;
			last = first;
		} else {
			// The list isn't empty
			if(last == first) {
				// The list has one element
				last = (Node*)sbrk(sizeof(Node));
				last->data = data;
				last->next = NULL;
				first->next = last;
			} else {
				// The list has more than one element
				Node* new_node = (Node*)sbrk(sizeof(Node));
				new_node->data = data;
				new_node->next = NULL;
				last->next = new_node;
				last = new_node;
			}
		}
	}

	T get(int index) {
		if(index == 0) {
			// Get the first element
			return this->first->data;
		} else {
			// Get the index'th element
			Node* curr = this->first;
			for(int i = 0; i < index; ++i) {
				curr = curr->next;
			}
			return curr->data;
		}
	}
	
	iterator begin(){
		return iterator(first);
	}
		
	iterator end(){
		return iterator(NULL);
	}
	
};
