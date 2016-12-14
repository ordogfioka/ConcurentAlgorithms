#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <system_error>

static const int M = 6;
static const int N = 1;
static unsigned long long NSTATES = 191102976;//4586471424L; //should be 24^(M*N)

// Global arrays
static int Up[24] = {22, 15, 18, 7, 10, 3, 17, 10, 10, -5, 2, -9, 9, -2, 5, -10, -10, -17, -3, -10, -7, -18, -15, -22};
static int Down[24] = {17, 22, 9, 18, 5, 10, 10, 15, -3, 10, -7, 2, -2, 7, -10, 3, -15, -10, -10, -5, -18, -9, -22, -17};
static int Left[24] = {10, 7, 14, 11, 18, 15, -2, -5, 9, 3, 13, 7, -7, -13, -3, -9, 5, 2, -15, -18, -11, -14, -7, -10};
static int Right[24] = {13, 18, 5, 15, 2, 7, 9, 14, -7, 11, -10, 3, -3, 10, -11, 7, -14, -9, -7, -2, -15, -5, -18, -13};

static int Pow24[7] = {1, 24, 576, 13824, 331776, 7962624, 191102976}; //, 4586471424L, 110075314176L, 2641807540224L;
//would try with atomics only
static std::vector<bool> known(NSTATES,false);
static std::vector<bool> newKnown(NSTATES,false);
static std::atomic<int> nNewKnown(0);

void n2tuple(unsigned long long k, std::vector<int>& ret)
{
  for (int i = 0; i < M*N; ++i) {
    ret[i] = (k % 24);
    k = k / 24;
  }
}
void worker(unsigned long long rangeStart,
            unsigned long long rangeEnd)
{
  std::vector<int> tuple(M*N);
  unsigned long long curr, j, k, neighb;
  for (curr = rangeStart; curr < rangeEnd; ++curr) {
    // For all known points
    if (known[curr]) {
      // Convert to tuple
      n2tuple(curr, tuple);
      std::cout << "doing work\n";

      // For all its neighbours, make them known, too.
      // For each row
      for (j = 0; j < M; ++j) {
        // Up neighbour
        neighb = curr;
        for (k = j*N; k < (j+1)*N; ++k) {
          neighb += Up[tuple[k]] * Pow24[k];
        }
        if (!newKnown[neighb]) {
          ++nNewKnown;
          newKnown[neighb] = true;
        }
        // Down neighbour
        neighb = curr;
        for (k = j*N; k < (j+1)*N; ++k) {
          neighb += Down[tuple[k]] * Pow24[k];
        }
        if (!newKnown[neighb]) {
          ++nNewKnown;
          newKnown[neighb] = true;
        }
      }
      // For each column
      for (j = 0; j < N; ++j) {
        // Left neighbour
        neighb = curr;
        for (k = j; k < M*N; k += N) {
          neighb += Left[tuple[k]] * Pow24[k];
        }
        if (!newKnown[neighb]) {
          ++nNewKnown;
          newKnown[neighb] = true;
        }
        // Right neighbour
        neighb = curr;
        for (k = j; k < M*N; k += N) {
          neighb += Right[tuple[k]] * Pow24[k];
        }
        if (!newKnown[neighb]) {
          ++nNewKnown;
          newKnown[neighb] = true;
        }
      }
    }
  }
  for (int i = 0; i < NSTATES; ++i) {
    if (newKnown[i] && !known[i]) {
      known[i] = true;
    }
  }
}
std::vector<bool> merger(std::vector<std::vector<bool> > knowns)
{
  std::vector<bool> ret(NSTATES,false);
  for(size_t i = 0; i < knowns.size();++i)
    {
      for (unsigned long long j = 0;j < NSTATES;++j)
        {
          ret[j] = ret[j] || knowns[i][j];
        }
    }
  return ret;
}

int main()
{
  auto start = std::chrono::system_clock::now();

  //threads init
  unsigned int thread_count = std::thread::hardware_concurrency();
  std::cout << "I will run on " << thread_count << " threads\n";
  std::vector<std::thread> workers(thread_count);

  //graph init
//  std::vector<bool> known(NSTATES,false);
//  std::vector<std::vector<bool>> knowns(thread_count,std::vector<bool>(NSTATES,false));
//  for (auto vec : knowns)
//    vec[0] = true;

//  std::vector<bool> newKnown(NSTATES,false);
//  std::vector<std::vector<bool>> newKnowns(thread_count,std::vector<bool>(NSTATES,false));
//  for (auto vec : newKnowns)
//    vec[0] = true;

  int nKnown = 0;
  std::atomic<int> nNewKnown(1);
  int level = 0;
  while (nKnown != nNewKnown) {
    std::cout << "There are " << (nNewKnown - nKnown) << " states on level " << level << " .\n";
    ++level;
    nKnown = nNewKnown;
    //copy data
//    for(auto vec : knowns)
//        std::copy(known.begin(),known.end(),vec.begin());

//    for(auto vec : newKnowns)
//        std::copy(newKnown.begin(),newKnown.end(),vec.begin());

    //start threads
    for (size_t i = 0; i < thread_count -1;++i)
      {
          workers.push_back(std::thread(worker,
                                        i*(NSTATES/thread_count),
                                        (i+1)*(NSTATES/thread_count)));
      }
    workers.push_back(std::thread(worker,
                                  (thread_count-1)*(NSTATES/thread_count),
                                  NSTATES));
    for(auto& t : workers)
      t.join();
    std::cout << std::endl;

  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "Altogether " << nKnown << " states reached.\n";
  std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
}
