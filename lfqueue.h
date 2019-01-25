#ifndef __lfqueue_h__
#define __lfqueue_h__

/* a non-blocking lock free queue
 * based on
 * Simple, fast, and practical non-blocking and blocking 
 * concurrent queue algorithms. PODC 96.
 */
template<typename T>
class lfqueue {
  public:
    struct node {
      node(){}
      node(const T& v):
	val{v},
	pos{0},
	next{nullptr}
      {}
      T val;
      int pos;
      node* next;
    };

    lfqueue():
      head{new node} {
	tail = head;
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
    bool pop();
    void push(const T& val);
  private:
    node* head, *tail;
};

#endif
