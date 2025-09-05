#include "include/ppm_io.cpp"
#include "matrix_meanshift.cpp"
#include "matrix_meanshift_omp.cpp"
#include "soa_meanshift.cpp"
#include "soa_meanshift_omp.cpp"
#include "rgb_pixels.cpp"
#include "image_matrix.h"
#include <iostream>
#include <chrono>

#define INPUT_PATH "../img/balloons_50.ppm"
#define OUTPUT_PATH "../img/out.ppm"
#define ITERATIONS 10
#define BANDWIDTH 0.4
#define RGB_CHANNELS 3

#include <omp.h>

// TODO: kernel multiplication

using namespace std::chrono;

int main()
{
	// open the ppm image
	PPM ppm;
	if (ppm.read(INPUT_PATH) != 0)
	{
		std::cout << "ERROR: failed to open the image";
		return -1;
	}
	int width = ppm.getW();
	int height = ppm.getH();
	int nOfPixels = width * height;
	uint8_t* inputBuffer = ppm.getImageHandler();

	// MATRIX MEANSHIFT START //

	// create the matrices
	int rgbPixelSize = RgbPixels::COLOR_SPACE_DIMENSION;
	int rgbxySpaceSize = RgbPixels::SPACE_DIMENSION;
	int rgbMaxValue = RgbPixels::MAX_VALUE;
	auto *matrix = new ImageMatrix(width, height, RGB_CHANNELS);
	auto modes = new float[nOfPixels * rgbxySpaceSize];

	// initialize the pixel data
	matrix->load(inputBuffer);

	// create the index array
	auto clusters = new int[nOfPixels];

	// create the result variables
	int nOfClusters;
	float totalTime = 0;

	// function loop
	printf("Using # %d threads.\n", omp_get_max_threads());
	for (int i = 0; i < ITERATIONS; ++i)
	{
		printf("Calling the MeanShift function... (%d)\n", i);

		// time the function
		auto start_time = high_resolution_clock::now();
		nOfClusters = matrixMeanShiftOmp(matrix->getPixels(), nOfPixels, BANDWIDTH, modes, clusters);
		auto end_time = high_resolution_clock::now();

		totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}

	float averageTime = totalTime / ITERATIONS;

	// print the results
	printf("Matrix timings: (measured on %d iterations)\n", ITERATIONS);
	printf("  total:   %fms\n", totalTime);
	printf("  average: %fms\n", averageTime);
	printf("Number of clusters: %d\n", nOfClusters);

    /*uint8_t outputBuffer[nOfPixels * rgbPixelSize];
    for (int i = 0; i < nOfPixels; ++i)
	{
		outputBuffer[i * rgbPixelSize]	   = (uint8_t) (modes[clusters[i] * rgbxySpaceSize]     * rgbMaxValue); // R
		outputBuffer[i * rgbPixelSize + 1] = (uint8_t) (modes[clusters[i] * rgbxySpaceSize + 1] * rgbMaxValue); // G
		outputBuffer[i * rgbPixelSize + 2] = (uint8_t) (modes[clusters[i] * rgbxySpaceSize + 2] * rgbMaxValue); // B
	}*/

	delete[] modes;

  	// MATRIX MEANSHIFT END //

	printf("\n");

	// SOA MEANSHIFT START //

    // create the structures of arrays
    RgbPixels soaPixels{};
	RgbPixels soaModes{};
	soaPixels.create(width, height);
	soaModes.create(width, height);

	// initialize the pixel data
	soaPixels.load(inputBuffer);

	// create the index array
    //int clusters[nOfPixels];

	// create the result variables
	//int nOfClusters;
	totalTime = 0;

	// function loop
	printf("Using # %d threads.\n", omp_get_max_threads());
	for (int i = 0; i < ITERATIONS; ++i)
	{
		printf("Calling the MeanShift function... (%d)\n", i);

		// time the function
		auto start_time = high_resolution_clock::now();
		nOfClusters = soaMeanShiftOmp(soaPixels, nOfPixels, BANDWIDTH, soaModes, clusters);
		auto end_time = high_resolution_clock::now();

		totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}

	averageTime = totalTime / ITERATIONS;

	// print the results
	printf("SoA timings: (measured on %d iterations)\n", ITERATIONS);
	printf("  total:   %fms\n", totalTime);
	printf("  average: %fms\n", averageTime);
	printf("Number of clusters: %d\n", nOfClusters);

	// create the output image buffer
	rgbPixelSize = RgbPixels::COLOR_SPACE_DIMENSION;
	rgbMaxValue = RgbPixels::MAX_VALUE;
    auto outputBuffer = new uint8_t[nOfPixels * rgbPixelSize];
    for(int i = 0; i < nOfPixels; ++i)
    {
		outputBuffer[i * rgbPixelSize]     = (uint8_t) (soaModes.r[clusters[i]] * rgbMaxValue); // R
		outputBuffer[i * rgbPixelSize + 1] = (uint8_t) (soaModes.g[clusters[i]] * rgbMaxValue); // G
		outputBuffer[i * rgbPixelSize + 2] = (uint8_t) (soaModes.b[clusters[i]] * rgbMaxValue); // B
    }

	// free the memory
	soaPixels.destroy();
	soaModes.destroy();
	delete[] clusters;

	// SOA MEANSHIFT END //

	ppm.load(outputBuffer, height, width, ppm.getMax(), ppm.getMagic());

	// write the output ppm image
	if (ppm.write(OUTPUT_PATH) != 0)
	{
		std::cout << "ERROR: failed to write the image";
		return -1;
	}

	delete[] outputBuffer;

	return 0;
}
