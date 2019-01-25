#include "lfqueue.h"
#include <iostream>
#include <vector>
#include <thread>

template<typename T>
void run(lfqueue<T>& q, int nthreaditems) {
  for (int i=1;i<=nthreaditems;++i) {
    q.push(i);
  }
}

bool test_queue_threaded(int nthreads, int nitems) {
  lfqueue<int> q;
  std::vector<std::thread> threads;
  for (int i=1;i<=nthreads;++i) {
    threads.push_back(std::thread{[&]{
	  run<int>(q, i);
	  }});
  }
  for (auto& t: threads) {
    t.join();
  }
  int nqitems=0;
  while (q.pop()) {
    ++nqitems;
  }
  return (nqitems == nitems*(nitems+1)/2);

}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: qtest nthreads nitems\n";
    return 1;
  }
  if (test_queue_threaded(atoi(argv[1]), atoi(argv[2]))) {
    std::cout << "OK!\n";
  }
  return 0;
}
