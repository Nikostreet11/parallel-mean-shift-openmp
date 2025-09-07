#ifndef PARALLEL_MEAN_SHIFT_OPENMP_EXPERIMENT_CPP
#define PARALLEL_MEAN_SHIFT_OPENMP_EXPERIMENT_CPP

#include "options.h"
#include "meanshift_seq.cpp"
#include "meanshift_omp.cpp"
#include "benchmark.cpp"

#include <omp.h>

uint8_t* imageSegmentationSeqExperiment(Ref<Image> pixels, uint8_t* input)
{
	// create the matrices
	Ref<Image> modes = pixels->clone();
	auto clusters = std::make_unique<int[]>(pixels->getSize());

	// initialize the pixel data
	pixels->load(input);

	printf("===== Using # 1 thread. =====\n");
	std::string name = "Sequential Mean-Shift [" + pixels->getImplementation() + "]";
	benchmark(name.c_str(), ITERATIONS, [&]()
	{
		return meanShiftSeq(pixels, BANDWIDTH, modes, clusters.get());
	});

	// map the pixels to obtain the segmented image
	pixels->map(modes, clusters.get());

	// create the output image input
	auto output = new uint8_t[pixels->getSize() * pixels->getChannels()];
	pixels->save(output);

	return output;
}

uint8_t* imageSegmentationOmpExperiment(Ref<Image> pixels, uint8_t* input)
{
	// create the matrices
	Ref<Image> modes = pixels->clone();
	auto clusters = std::make_unique<int[]>(pixels->getSize());

	// initialize the pixel data
	pixels->load(input);

	printf("===== Using # %d threads. =====\n", omp_get_max_threads());
	std::string name = "Parallel Mean-Shift [" + pixels->getImplementation() + "]";
	benchmark(name.c_str(), ITERATIONS, [&]()
	{
		return meanShiftOmp(pixels, BANDWIDTH, modes, clusters.get());
	});

	// map the pixels to obtain the segmented image
	pixels->map(modes, clusters.get());

	// create the output image input
	auto output = new uint8_t[pixels->getSize() * pixels->getChannels()];
	pixels->save(output);

	return output;
}

#endif // PARALLEL_MEAN_SHIFT_OPENMP_EXPERIMENT_CPP