#include <chrono>
#include <functional>
#include <cstdio>
#include <omp.h>
using namespace std::chrono;

template <typename Func>
void benchmark(const char* name, Func func, int iterations = 10)
{
	int nOfClusters = 0;
	float totalTime = 0;

	printf("Using # %d threads.\n", omp_get_max_threads());
	for (int i = 0; i < iterations; ++i)
	{
		printf("Calling %s... (%d)\n", name, i);

		auto start_time = high_resolution_clock::now();
		nOfClusters = func(); // call your function here
		auto end_time = high_resolution_clock::now();

		totalTime += (float)duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}

	float averageTime = totalTime / iterations;

	printf("%s timings: (measured on %d iterations)\n", name, iterations);
	printf("  total:   %fms\n", totalTime);
	printf("  average: %fms\n", averageTime);
	printf("Number of clusters: %d\n\n", nOfClusters);
}
