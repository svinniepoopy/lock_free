#ifndef __lfqueue_h__
#define __lfqueue_h__

/* a non-blocking lock free queue
 * based on
 * Simple, fast, and practical non-blocking and blocking 
 * concurrent queue algorithms. PODC 96.
 */

#include <iostream>
#include <thread>
#include <atomic>

namespace lfatomic {
  template<typename T>
    bool compare_and_swap(T** owner, const T* expected, T** desired) {
      if (*owner == *desired) {
	return true;
      }
      if (*owner == expected) {
	*owner = *desired;
	return true;
      }
      return false;
    }
}; // namespace

using namespace lfatomic;

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

    lfqueue() {
	head = new node;
	tail = head; 
	tail->next = nullptr;
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
      std::cout << "~lfqueue()\n";
    }    

    const node *const front() const {
      return head;
    }

    bool pop2() {
      std::atomic<node*> HEAD{head};
      std::atomic<node*> TAIL{tail};
      std::atomic<node*> hd{nullptr};
      while (1) {
	hd = HEAD.load();
	std::atomic<node*> tl{TAIL.load()};
	std::atomic<node*> nxt{HEAD.load()->next};
	if (hd == HEAD.load()) {
	  if (HEAD.load() == TAIL.load()) {
	    if (nxt.load() == nullptr) {
	      return false;
	    }
	    node* _tl_ = tl.load();
	    TAIL.compare_exchange_strong(_tl_, nxt.load(),
		std::memory_order_release,
		std::memory_order_relaxed); 
	  } else {
	    node* _hd_ = hd.load();
	    if (HEAD.compare_exchange_strong(_hd_, nxt.load(),
		  std::memory_order_release,
		  std::memory_order_relaxed)) { 
	      break;
	    }
	  }
	}
      }
      delete hd.load();
      return true;
    }

    bool pop() {
      // std::thread::id this_id = std::this_thread::get_id();
      // std::cout << "thd " << this_id << " in pop\n";
      node* hd=nullptr;
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
	    if (compare_and_swap(&head, hd, &nxt)) {
	      break;
	    }
	  }
	}
      }
      if (hd) {
	delete hd;
      }
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
	    if (compare_and_swap(&tail->next, nxt, &newnod)) {
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
