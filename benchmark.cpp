#ifndef PARALLEL_MEAN_SHIFT_OPENMP_BENCHMARK_CPP
#define PARALLEL_MEAN_SHIFT_OPENMP_BENCHMARK_CPP

#include <chrono>
#include <cstdio>

using namespace std::chrono;

template <typename Func>
void benchmark(const char* name, int iterations, Func func)
{
	int nOfClusters = 0;
	float totalTime = 0;

	for (int i = 0; i < iterations; ++i)
	{
		printf("Running %s... (%d)\n", name, i);

		auto start_time = high_resolution_clock::now();
		nOfClusters = func();
		auto end_time = high_resolution_clock::now();

		totalTime += (float)duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}

	float averageTime = totalTime / iterations;

	printf("Results: (measured on %d iterations)\n", iterations);
	printf("  Total:   %fms\n", totalTime);
	printf("  Average: %fms\n", averageTime);
	printf("Number of clusters: %d\n\n", nOfClusters);
}

#endif // PARALLEL_MEAN_SHIFT_OPENMP_BENCHMARK_CPP