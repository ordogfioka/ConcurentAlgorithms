#include <stdio.h>
#include <iostream>
#include <vector>

#define M 6
#define N 1
#define NSTATES 24*24*24*24*24*24 // Should be 24^(N*M)

using namespace std;

// Global arrays
int Up[24] = {22, 15, 18, 7, 10, 3, 17, 10, 10, -5, 2, -9, 9, -2, 5, -10, -10, -17, -3, -10, -7, -18, -15, -22};
int Down[24] = {17, 22, 9, 18, 5, 10, 10, 15, -3, 10, -7, 2, -2, 7, -10, 3, -15, -10, -10, -5, -18, -9, -22, -17};
int Left[24] = {10, 7, 14, 11, 18, 15, -2, -5, 9, 3, 13, 7, -7, -13, -3, -9, 5, 2, -15, -18, -11, -14, -7, -10};
int Right[24] = {13, 18, 5, 15, 2, 7, 9, 14, -7, 11, -10, 3, -3, 10, -11, 7, -14, -9, -7, -2, -15, -5, -18, -13};

int Pow24[7] = {1, 24, 576, 13824, 331776, 7962624, 191102976}; //, 4586471424L, 110075314176L, 2641807540224L;

void n2tuple(int k, vector<int>& ret)
{
  for (int i = 0; i < M*N; ++i) {
    ret[i] = (k % 24);
    k = k / 24;
  }
}

int main()
{
  vector<bool> known(NSTATES, false);
  known[0] = true;

  vector<bool> newKnown(NSTATES, false);
  newKnown[0] = true;

  int nKnown = 0;
  int nNewKnown = 1;
  int level = 0;

  vector<int> tuple(M*N);

  while (nKnown != nNewKnown) {
    cout << "There are " << (nNewKnown - nKnown) << " states on level " << level << " .\n";
    ++level;
    nKnown = nNewKnown;
    int curr, j, k, neighb, ;
    for (curr = 0; curr < NSTATES; ++curr) {
      // For all known points
      if (known[curr]) {
	// Convert to tuple
	n2tuple(curr, tuple);
	
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
    cout << endl;

  }
  cout << "Altogether " << nKnown << " states reached.\n";
}
