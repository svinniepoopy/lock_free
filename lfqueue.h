#ifndef __lfqueue_h__
#define __lfqueue_h__

/* a non-blocking lock free queue
 * based on
 * Simple, fast, and practical non-blocking and blocking 
 * concurrent queue algorithms. PODC 96.
 */

namespace {
    template<typename T>
	bool compare_and_swap(T** owner, const T* expected, T** desired) {
	    if (*owner == expected) {
		*owner = *desired;
		return false;
	    }
	    return true;
	}
}; // namespace

template<typename T>
class lfqueue {
    public:
	struct node {
	    node():
		pos{0},
		next{nullptr}
	    {}
	    node(const T& v, int p=0):
		val{v},
		pos{p},
		next{nullptr}
	    {}
	    T val;
	    int pos;
	    node* next;
	};

	lfqueue():
	    head{new node} {
		tail = head;
		tail->next = head->next = nullptr;
	    }

	~lfqueue() {
	    node* tmp;
	    if (head) {
		tmp = head->next;
		delete head;
		head = tmp;
	    }
	    head = 0;
	    tail = 0;
	}    

	const node* front() const {
	    return head;
	}
	bool pop() {
	    node* hd;
	    while (1) {
		hd = head;
		node* tl = tail;
		node* nxt = head->next;
		if (hd == head) {
		    if (head == tail) {
			if (nxt == nullptr) {
			    return false;
			}
			compare_and_swap(&tail, tl, &nxt);
		    } else {
			node* _nxt_ = nxt;
			if (!compare_and_swap(&head, hd, &nxt)) {
			    break;
			}
		    }
		}
	    }
	    delete hd;
	    return true;
	}

	void push(const T& val) {
	    const node* const front_node = front();
	    node* newnod = new node{val,front_node->pos+1};
	    newnod->next = nullptr;
	    node* tl;
	    while (1) {
		tl = tail;
		node* nxt = tail->next;
		if (tl == tail) { 
		    if (nxt == nullptr) {	// tail is pointing to the last node
			if (!compare_and_swap(&tail->next, nxt, &newnod)) {
			    break;
			}
		    } else {	// tail was not pointing to the last node
			compare_and_swap(&tail, tl, &nxt);  
		    }
		}
	    }
	    compare_and_swap(&tail, tl, &newnod);
	}
    private:
	node* head, *tail;
};

#endif
