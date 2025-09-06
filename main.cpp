#include "options.h"

#include "image_matrix.h"
#include "image_soa.h"
//#include "matrix_meanshift.cpp"
#include "matrix_meanshift_omp.cpp"
//#include "soa_meanshift.cpp"
#include "soa_meanshift_omp.cpp"

#include "include/ppm_io.cpp"

#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std::chrono;

// TODO: kernel multiplication

int main()
{
	PPM image;
    
    // read the input image
	if (image.read(INPUT_PATH) != 0)
	{
		return -1;
	}

    // get the raw image buffer
	uint8_t* buffer = image.getImageHandler();
    
	// MATRIX MEANSHIFT START //

	// create the matrices
	auto matrix = std::make_shared<ImageMatrix>(image.getW(), image.getH());
	auto modes = new float[image.getW() * image.getH() * (RGB_CHANNELS + 2)];

	// initialize the pixel data
	matrix->load(buffer);

	// create the index array
	auto clusters = new int[image.getW() * image.getH()];

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
		nOfClusters = matrixMeanShiftOmp(matrix->getPixels(), image.getW() * image.getH(), BANDWIDTH, modes, clusters);
		auto end_time = high_resolution_clock::now();

		totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}

	float averageTime = totalTime / ITERATIONS;

	// print the results
	printf("Matrix timings: (measured on %d iterations)\n", ITERATIONS);
	printf("  total:   %fms\n", totalTime);
	printf("  average: %fms\n", averageTime);
	printf("Number of clusters: %d\n", nOfClusters);

    /*uint8_t outputBuffer[image.getW() * image.getH() * rgbPixelSize];
    for (int i = 0; i < image.getW() * image.getH(); ++i)
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
    auto soa_pixels = std::make_shared<ImageSoa>(image.getW(), image.getH());
    auto soa_modes = std::make_shared<ImageSoa>(image.getW(), image.getH());

	// initialize the pixel data
    soa_pixels->load(buffer);

	// clean the result variables
    nOfClusters = 0;
	totalTime = 0;

	// function loop
	printf("Using # %d threads.\n", omp_get_max_threads());
	for (int i = 0; i < ITERATIONS; ++i)
	{
		printf("Calling the MeanShift function... (%d)\n", i);

		// time the function
		auto start_time = high_resolution_clock::now();
		nOfClusters = soaMeanShiftOmp(soa_pixels, image.getW() * image.getH(), BANDWIDTH, soa_modes, clusters);
		auto end_time = high_resolution_clock::now();

		totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
	}
	averageTime = totalTime / ITERATIONS;

	// print the results
	printf("SoA timings: (measured on %d iterations)\n", ITERATIONS);
	printf("  total:   %fms\n", totalTime);
	printf("  average: %fms\n", averageTime);
	printf("Number of clusters: %d\n", nOfClusters);

    // map the pixels to obtain the segmented image
    soa_pixels->map(soa_modes, clusters);

	// create the output image buffer
    auto outputBuffer = new uint8_t[image.getW() * image.getH() * RGB_CHANNELS];
    soa_pixels->save(outputBuffer);

	// SOA MEANSHIFT END //
    
    // load the segmented buffer
	image.load(outputBuffer, image.getH(), image.getW(), image.getMax(), image.getMagic());

	// write the image to output file
	if (image.write(OUTPUT_PATH) != 0)
	{
		return -1;
	}

    delete[] clusters;
	delete[] outputBuffer;

	return 0;
}
