
template <class T>
class LinkedList {

public:
	class Node {
	public:
	   T data;
	   Node* next;
	};

	class Iterator {
		Node* node_ptr;

	public:
      Iterator() : node_ptr(NULL) {}

		Iterator(Node* node_ptr) : node_ptr(node_ptr) {}

      T* operator->() const {
         return &node_ptr->data;
      }

      T& operator*() const {
         return node_ptr->data;
      }

		Iterator& operator=(const Iterator& Iterator ) {
			if (node_ptr == NULL) {
            return *this;
         }
			node_ptr = Iterator.node_ptr;
			return *this;
		}

		bool operator==(const Iterator& Iterator) {
			if (node_ptr == NULL && Iterator.node_ptr == NULL) {
            return true;
         }
			if (node_ptr == NULL || Iterator.node_ptr == NULL) {
            return false;
         }
			return node_ptr == Iterator.node_ptr;
		}

		bool operator!=(const Iterator& Iterator) {
			return !(*this == Iterator);
		}

		Iterator& operator++() {
			if (node_ptr == NULL) {
            return *this;
         }
			node_ptr = node_ptr->next;
			return *this;
		}

		const T& operator*() {
			return node_ptr->data;
		}
    };


	Node* first;
	Node* last;

	LinkedList<T>(){
		first = NULL;
		last = NULL;
	}

   bool isEmpty() {
      return first == NULL;
   }

	bool add(T data) {
		if (isEmpty()) {
			// The list is empty
			first = (Node*)sbrk(sizeof(Node));
         if (first == (void*)(-1)) {
            return false;
         }
			first->data = data;
			first->next = NULL;
			last = first;
		} else {
			// The list isn't empty
			if (last == first) {
				// The list has one element
				last = (Node*)sbrk(sizeof(Node));
            if (last == (void*)(-1)) {
               return false;
            }
				last->data = data;
				last->next = NULL;
				first->next = last;
			} else {
				// The list has more than one element
				Node* new_node = (Node*)sbrk(sizeof(Node));
            if (new_node == (void*)(-1)) {
               return false;
            }
				new_node->data = data;
				new_node->next = NULL;
				last->next = new_node;
				last = new_node;
			}
		}
      return true;
	}

	T get(int index) {
		if (index == 0) {
			// Get the first element
			return this->first->data;
		} else {
			// Get the index'th element
			Node* curr = this->first;
			for (int i = 0; i < index; ++i) {
				curr = curr->next;
			}
			return curr->data;
		}
	}

	Iterator begin(){
		return Iterator(first);
	}

	Iterator end(){
		return Iterator(NULL);
	}

};
