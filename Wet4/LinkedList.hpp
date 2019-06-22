template <class T>
class Node {
   T data;
   Node<T>* next;
};

template <class T>
class LinkedList {
public:
	Node<T>* first;
	Node<T>* last;
	LinkedList<T>() {
		first = NULL;
		last = NULL;
	}

	void add(T data) {
		if(!first) {
			// The list is empty
			first = new Node<T>;
			first->data = data;
			first->next = NULL;
			last = first;
		} else {
			// The list isn't empty
			if(last == first) {
				// The list has one element
				last = new Node<T>;
				last->data = data;
				last->next = NULL;
				first->next = last;
			} else {
				// The list has more than one element
				Node<T>* new_node = new Node<T>;
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
			Node<T>* curr = this->first;
			for(int i = 0; i < index; ++i) {
				curr = curr->next;
			}
			return curr->data;
		}
	}

};
