#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

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

vector<bool> known(NSTATES, false);
vector<bool> newKnown(NSTATES, false);
vector<int> myTuple(M*N);
atomic<int> nNewKnown;
int THREADS = 3;

void n2myTuple(int k, vector<int>& ret)
{
	for (int i = 0; i < M*N; ++i) 
	{
		ret[i] = (k % 24);
		k = k / 24;
	}
}

void doit(int start,int end)
{
	int myNewKnown = 0;
	int curr, j, k, neighb;
	for (curr = 0; curr < NSTATES; ++curr) 
	{
		// For all known points
		if (known[curr]) 
		{
			// Convert to myTuple
			n2myTuple(curr, myTuple);

			// For all its neighbours, make them known, too.
			// For each row
			for (j = 0; j < M; ++j) 
			{
				// Up neighbour
				neighb = curr;
				for (k = j*N; k < (j+1)*N; ++k) 
				{
					neighb += Up[myTuple[k]] * Pow24[k];
				}
				if (!newKnown[neighb]) 
				{
					++myNewKnown; 
					newKnown[neighb] = true;
				}
				// Down neighbour
				neighb = curr;
				for (k = j*N; k < (j+1)*N; ++k) 
				{
					neighb += Down[myTuple[k]] * Pow24[k];
				}
				if (!newKnown[neighb]) 
				{
					++myNewKnown; 
					newKnown[neighb] = true;
				}
			}
			// For each column
			for (j = 0; j < N; ++j) 
			{
				// Left neighbour
				neighb = curr;
				for (k = j; k < M*N; k += N) 
				{
					neighb += Left[myTuple[k]] * Pow24[k];
				}
				if (!newKnown[neighb]) 
				{
					++myNewKnown; 
					newKnown[neighb] = true;
				}
				// Right neighbour
				neighb = curr;
				for (k = j; k < M*N; k += N) 
				{
					neighb += Right[myTuple[k]] * Pow24[k];
				}
				if (!newKnown[neighb]) 
				{
					++myNewKnown; 
					newKnown[neighb] = true;
				}
			}
		}
	}
	nNewKnown += myNewKnown;
}

int main()
{
	known[0] = true;
	newKnown[0] = true;

	int nKnown = 0;
	nNewKnown = 1;
	int level = 0;
	auto start = std::chrono::steady_clock::now();
	
	while (nKnown != nNewKnown) 
	{
		cout << "There are " << (nNewKnown - nKnown) << " states on level " << level << " .\n";
		++level;
		nKnown = nNewKnown;
		
		//ide a parhuzamositast
		std::vector<std::thread> results;
		for (size_t i = 0; i < THREADS-1; i++)
		{
			results.push_back(thread(doit,i*(NSTATES/THREADS),(i+1)*(NSTATES/THREADS) ));
		}
		results.push_back(thread(doit,(NSTATES-1)*(NSTATES/THREADS),NSTATES ));

		for (size_t i = 0; i < THREADS; i++)
		{
			results[i].join();
		}
		
		for (int i = 0; i < NSTATES; ++i) 
		{
			if (newKnown[i] && !known[i]) 
			{
				known[i] = true;
			}
		}
		cout << endl;

	}
	auto end = std::chrono::steady_clock::now();
	cout << "Altogether " << nKnown << " states reached.\n";
	std::chrono::duration<double> diff = end-start;
    std::cout << "Time required:" << diff.count() << " s\n";
}