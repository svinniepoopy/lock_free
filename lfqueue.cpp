#include "lfqueue.h"

namespace {
template<typename T>
bool compare_and_swap(T** owner, const T* expected, T** desired) {
  if (*owner == *desired) {
    return false;
  }
  if (*owner == expected) {
    *owner = *desired;
    return false;
  }
  return true;
}
}; // namespace


template<typename T>
bool lfqueue<T>::pop() {
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
	compare_and_swap(&tail, tl, nxt);
      } else {
	node* _nxt_ = nxt;
	if (compare_and_swap(&head, hd, nxt)) {
	  break;
	}
      }
    }
  }
  delete hd;
  return true;
}

template<typename T>
void lfqueue<T>::push(const T& val) {
  const node* const front_node = front();
  node* newnod = new node{val,++front_node->pos};
  newnod->next = nullptr;
  node* tl;
  while (1) {
    * tl = tail;
    node* nxt = tail->next;
    if (tl == tail) { 
      if (nxt == nullptr) {	// tail is pointing to the last node
	if (compare_and_swap(&tail->next, nxt, &newnod)) {
	  break;
	}
      } else {	// tail was not pointing to the last node
	compare_and_swap(&tail, tl, nxt);  
      }
    }
  }
  compare_and_swap(&tail, tl, newnod);
} 
