#include "lfqueue.h"
#include <iostream>
#include <vector>
#include <thread>
#include <cassert>

template<typename T>
void run(lfqueue<T>& q, int nthreaditems) {
  for (int i=1;i<=nthreaditems;++i) {
    q.push(i);
  }
}

template<typename T>
bool qpop(lfqueue<T>& q) {
  return q.pop2();
}

bool test_queue2() {
  int nthreads=10000,nitems=3;
  lfqueue<int> q;
  std::vector<std::thread> threads;
  for (int i=1;i<=nthreads;++i) {
    threads.push_back(std::thread{[&]{
	  run<int>(q, nitems);
	  }});
  }
  for (auto& t: threads) {
    t.join();
  }
  int nqitems=0;
  std::vector<std::thread> popthreads;
  for (int i=0;i<nthreads;++i) {
    popthreads.push_back(std::thread{[&]{
	qpop<int>(q);
	}});
  }
  return true;
}

bool test_queue() {
  int nthreads=2,nitems=250;
  lfqueue<int> q;
  std::vector<std::thread> threads;
  for (int i=1;i<=nthreads;++i) {
    threads.push_back(std::thread{[&]{
	  run<int>(q, nitems);
	  }});
  }
  for (auto& t: threads) {
    t.join();
  }
  int nqitems=0;
  while (q.pop()) {
    ++nqitems;
  }
  std::cout << nqitems << ',' << nthreads*nitems << '\n';
  return (nqitems == nthreads*nitems); 
}

using node = lfqueue<int>::node;
void test_CAS1() {
  node* owner = new node(42);
  node* expected = owner;
  node* desired = nullptr; 
  bool is_swapped = compare_and_swap(&owner, expected, &desired);
  assert(is_swapped);
  assert(owner == desired);
  delete expected;
}

void test_CAS2() {
  node* owner = new node(1);
  node* _owner_ = owner;
  node* expected = new node(3);
  node* _expected_ = expected;
  node* desired = new node(5);;
  node* _desired_ = desired;
  bool is_swapped = compare_and_swap(&owner, expected, &desired);
  assert(!is_swapped);
  assert(owner == _owner_);
  assert(expected == _expected_);
  assert(desired == _desired_);
  delete owner;
  delete expected;
  delete desired;
}

void test_CAS3() {
  node* owner = new node(1);
  node* _owner_ = owner;
  node* expected = owner;
  node* desired = new node(5);
  node* _desired_ = desired;
  bool is_swapped = compare_and_swap(&owner, expected, &desired);
  assert(is_swapped);
  assert(owner = _owner_);
  assert(desired = _desired_);
  delete owner;
  delete desired;
}

using namespace lfatomic;
int test_CAS() {
  test_CAS1();
  test_CAS2();
  test_CAS3();
  return 0;
}

int main(int argc, char** argv) {
  if (test_CAS() < 0) {
    std::cerr << "test_CAS OK!\n";
    return 1;
  }

  assert(test_queue());
  return 0;
}
