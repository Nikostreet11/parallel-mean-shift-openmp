#include "include/ppm_io.cpp"
#include "matrix_meanshift.cpp"
#include "matrix_meanshift_omp.cpp"
#include "soa_meanshift.cpp"
#include "soa_meanshift_omp.cpp"
#include "rgb_pixels.cpp"
#include <iostream>
#include <chrono>

#define INPUT_PATH "../img/balloons_100.ppm"
#define OUTPUT_PATH "../img/out.ppm"
#define ITERATIONS 1
#define BANDWIDTH 0.4

#include <omp.h>

/* ----- TIMINGS ------------------------------
 * 100x100 image, Windows, 12 cores, 18 threads
 * 	 Matrix sequential: 3609ms
 * 	 Matrix OpenMP:		1029ms
 * 	   Speedup: 		3.5
 *   SoA sequential:	3834ms
 * 	 SoA OpenMP:		1060ms
 * 	   Speedup: 		3.6
 *
 * 100x100 image, Linux, 8 cores, 12 threads
 * 	 Matrix sequential: 2461ms
 * 	 Matrix OpenMP: 998ms
 * 	   Speedup: 2.5
 *   SoA sequential: 2711ms
 * 	 SoA OpenMP: 726ms
 * 	   Speedup: 3.7
 *
 * Averaged on 10 iterations
 * --------------------------------------------
 */

// todo: cluster in the HSV space
// todo: cluster in the L*U*V* space
// todo: kernel multiplication
// todo: parallelize using Cuda

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
    auto pixels = new float[nOfPixels * rgbxySpaceSize];
	auto modes = new float[nOfPixels * rgbxySpaceSize];

	// initialize the pixel data
    for (int i = 0; i < nOfPixels; ++i)
	{
		pixels[i * rgbxySpaceSize]     = (float) inputBuffer[i * rgbPixelSize]     / rgbMaxValue; // R
        pixels[i * rgbxySpaceSize + 1] = (float) inputBuffer[i * rgbPixelSize + 1] / rgbMaxValue; // G
        pixels[i * rgbxySpaceSize + 2] = (float) inputBuffer[i * rgbPixelSize + 2] / rgbMaxValue; // B
        pixels[i * rgbxySpaceSize + 3] = (float) ((i) % width) / (width - 1);					  // X
        pixels[i * rgbxySpaceSize + 4] = (float) ((i) / width) / (height - 1);					  // Y
    }

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
		nOfClusters = matrixMeanShiftOmp(pixels, nOfPixels, BANDWIDTH, modes, clusters);
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

	delete[] pixels;
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
