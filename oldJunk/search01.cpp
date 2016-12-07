#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>

#define M 6
#define N 1
#define NSTATES 24*24*24*24*24*24 // Should be 24^(N*M)
#define THREAD_COUNT 8


// Global arrays
int Up[24] = { 22, 15, 18, 7, 10, 3, 17, 10, 10, -5, 2, -9, 9, -2, 5, -10, -10, -17, -3, -10, -7, -18, -15, -22 };
int Down[24] = { 17, 22, 9, 18, 5, 10, 10, 15, -3, 10, -7, 2, -2, 7, -10, 3, -15, -10, -10, -5, -18, -9, -22, -17 };
int Left[24] = { 10, 7, 14, 11, 18, 15, -2, -5, 9, 3, 13, 7, -7, -13, -3, -9, 5, 2, -15, -18, -11, -14, -7, -10 };
int Right[24] = { 13, 18, 5, 15, 2, 7, 9, 14, -7, 11, -10, 3, -3, 10, -11, 7, -14, -9, -7, -2, -15, -5, -18, -13 };

int Pow24[7] = { 1, 24, 576, 13824, 331776, 7962624, 191102976 }; //, 4586471424L, 110075314176L, 2641807540224L;



void n2tuple(int k, std::vector<int>& ret)
{
	for (int i = 0; i < M*N; ++i)
	{
		ret[i] = (k % 24);
		k = k / 24;
	}
}

void getNeighbours(int& curr, int& neighb, std::vector<bool> &newKnown, std::vector<int> &tuple, std::vector<int>& neighbours)
{
	neighbours.clear();
	
	// For all its neighbours, make them known, too.
	// For each row
	for (int j = 0; j < M; ++j)
	{
		// Up neighbour
		neighb = curr;
		for (int k = j*N; k < (j + 1)*N; ++k)
		{
			neighb += Up[tuple[k]] * Pow24[k];
		}
		if (!newKnown[neighb])
		{
			neighbours.push_back(neighb);
		}
		// Down neighbour
		neighb = curr;
		for (int k = j*N; k < (j + 1)*N; ++k)
		{
			neighb += Down[tuple[k]] * Pow24[k];
		}
		if (!newKnown[neighb])
		{
			neighbours.push_back(neighb);
		}
	}
	// For each column
	for (int j = 0; j < N; ++j)
	{
		// Left neighbour
		neighb = curr;
		for (int k = j; k < M*N; k += N)
		{
			neighb += Left[tuple[k]] * Pow24[k];
		}
		if (!newKnown[neighb])
		{
			neighbours.push_back(neighb);
		}
		// Right neighbour
		neighb = curr;
		for (int k = j; k < M*N; k += N)
		{
			neighb += Right[tuple[k]] * Pow24[k];
		}
		if (!newKnown[neighb])
		{
			neighbours.push_back(neighb);
		}
	}
}


void findNeightbours(int from, int to, std::vector<bool> &known, std::vector<bool> &newKnown, std::atomic<int> &nNewKnown)
{
	std::vector<int> tuple(M*N);
	std::vector<int> neighbours;
	int neighb;
	int found = 0;
	for (int curr = from; curr < to; ++curr)
	{
		// For all known points
		if (known[curr])
		{
			// Convert to tuple
			n2tuple(curr, tuple);
			// ref-fel kevesebb mem�ria foglal�s van, jobb mint ha a fv hozn� l�tre, mert akk minden megh�v�sn�l teljesen �jat csin�lna, �gy pedig csak egy van 
			getNeighbours(std::ref(curr), std::ref(neighb), std::ref(newKnown), std::ref(tuple), std::ref(neighbours));
			found += neighbours.size();

			for (auto n : neighbours)
			{
				newKnown[n] = true;
			}
			
		}
	}
	// std::cout << nNewKnown << " + " << found << " = ";
	nNewKnown += found;
	// std::cout << nNewKnown << "\n";
}

int main(int argc, char** argv)
{
	std::vector<bool> known(NSTATES, false);
	known[0] = true;

	std::vector<bool> newKnown(NSTATES, false);
	newKnown[0] = true;

	int nKnown = 0;
	std::atomic<int> nNewKnown(1);
	int level = 0;

	auto start = std::chrono::system_clock::now();

	while (nKnown != nNewKnown)
	{
		std::cout << "There are " << (nNewKnown - nKnown) << " states on level " << level << " .\n";
		++level;
		nKnown = nNewKnown;

		std::thread threads[THREAD_COUNT - 1];
		int from = 0;
		int to;
		int interval = NSTATES / THREAD_COUNT;

		for (int i = 0; i < THREAD_COUNT - 1; ++i)
		{
			to = NSTATES - (THREAD_COUNT - (i + 1))*interval;
			threads[i] = std::thread(findNeightbours, from, to, std::ref(known), std::ref(newKnown), std::ref(nNewKnown));
			from = to;
		}

		findNeightbours(from, NSTATES, std::ref(known), std::ref(newKnown), std::ref(nNewKnown));

		for (int i = 0; i < THREAD_COUNT - 1; ++i)
		{
			threads[i].join();
		}

		for (int i = 0; i < NSTATES; ++i)
		{
			if (newKnown[i] && !known[i])
			{
				known[i] = true;
			}
		}
		std::cout << std::endl;

	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "Altogether " << nKnown << " states reached.\n";

	std::cout << "Elapsed time: " << elapsed_seconds.count() << "s\n";
}
