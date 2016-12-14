#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>

#define M 6
#define N 1
#define NSTATES 24*24*24*24*24*24 // Should be 24^(N*M)

// Global arrays
static int Up[24] = {22, 15, 18, 7, 10, 3, 17, 10, 10, -5, 2, -9, 9, -2, 5, -10, -10, -17, -3, -10, -7, -18, -15, -22};
static int Down[24] = {17, 22, 9, 18, 5, 10, 10, 15, -3, 10, -7, 2, -2, 7, -10, 3, -15, -10, -10, -5, -18, -9, -22, -17};
static int Left[24] = {10, 7, 14, 11, 18, 15, -2, -5, 9, 3, 13, 7, -7, -13, -3, -9, 5, 2, -15, -18, -11, -14, -7, -10};
static int Right[24] = {13, 18, 5, 15, 2, 7, 9, 14, -7, 11, -10, 3, -3, 10, -11, 7, -14, -9, -7, -2, -15, -5, -18, -13};

static int Pow24[7] = {1, 24, 576, 13824, 331776, 7962624, 191102976}; //, 4586471424L, 110075314176L, 2641807540224L;

void n2tuple(int k, std::vector<int>& ret)
{
  for (int i : ret)
    {
      i = (k % 24);
      k = k / 24;
    }
}

int main()
{
  std::vector<bool> known(NSTATES, false);
  known[0] = true;

  std::unordered_set<int> newKnown, prevLevel;
  newKnown.insert(0);

  int nNewKnown = 1;
  int nKnown = 1;
  int level = 0;

  std::vector<int> tuple(M*N);

  while (0 != nNewKnown) {
      std::cout << "There are " << (nNewKnown) << " states on level " << level << " .\n";
      ++level;
      int j,k,neighb;
      prevLevel = newKnown;
      newKnown.clear();
      for (auto i : prevLevel)
        {
          // Convert to tuple
          n2tuple(i, tuple);

          // For all its neighbours, make them known, too.
          // For each row
          for (j = 0; j < M; ++j) {
              // Up neighbour
              neighb = i;
              for (k = j*N; k < (j+1)*N; ++k) {
                  neighb += Up[tuple[k]] * Pow24[k];
                  std::cout << "Up:" << neighb << std::endl;
                }
              if (!known[neighb]) {
                  newKnown.insert(neighb);
                }
              // Down neighbour
              neighb = i;
              for (k = j*N; k < (j+1)*N; ++k) {
                  neighb += Down[tuple[k]] * Pow24[k];
                }
              if (!known[neighb]) {
                  newKnown.insert(neighb);
                }	}
          // For each column
          for (j = 0; j < N; ++j) {
              // Left neighbour
              neighb = i;
              for (k = j; k < M*N; k += N) {
                  neighb += Left[tuple[k]] * Pow24[k];
                }
              if (!known[neighb]) {
                  newKnown.insert(neighb);
                }	  // Right neighbour
              neighb = i;
              for (k = j; k < M*N; k += N) {
                  neighb += Right[tuple[k]] * Pow24[k];
                }
              if (!known[neighb]) {
                  newKnown.insert(neighb);
                }
            }
        }

      for (auto i : newKnown)
        {
          known[i] = true;
        }
      nNewKnown = newKnown.size();
      nKnown += nNewKnown;
    }

  std::cout << "Altogether " << nKnown << " states reached.\n";
}
