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

bool test_queue() {
  int nthreads=10000,nitems=100;
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

  return (nqitems == nthreads*nitems); 
}

using namespace lfatomic;
int test_CAS() {
  using node = lfqueue<int>::node;
  node* owner = new node(42);
  node* expected = owner;
  node* desired = nullptr; 
  bool is_swapped = compare_and_swap(&owner, expected, &desired);
  assert(is_swapped);
  assert(owner == desired);
  delete expected;
  //
  owner = new node(1);
  node* _owner_ = owner;
  expected = new node(3);
  node* _expected_ = expected;
  desired = new node(5);;
  node* _desired_ = desired;
  is_swapped = compare_and_swap(&owner, expected, &desired);
  assert(!is_swapped);
  assert(owner == _owner_);
  assert(expected == _expected_);
  assert(desired == _desired_);
  delete owner;
  delete expected;
  delete desired;
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
