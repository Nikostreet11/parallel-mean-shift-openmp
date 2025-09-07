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

uint8_t* matrixOmpImageSegmentationRun(uint8_t* input, int width, int height)
{

    // create the matrices
    auto matrixPixels = std::make_shared<ImageMatrix>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
    auto matrixModes = std::make_shared<ImageMatrix>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
    //auto matrixModes = new float[width * height * (RGB_CHANNELS + 2)];

    // initialize the pixel data
    matrixPixels->load(input);

    // create the index array
    auto clusters = new int[width * height];

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
        nOfClusters = matrixMeanShiftOmp(matrixPixels, width * height, BANDWIDTH, matrixModes, clusters);
        auto end_time = high_resolution_clock::now();

        totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
    }

    float averageTime = totalTime / ITERATIONS;

    // print the results
    printf("Matrix timings: (measured on %d iterations)\n", ITERATIONS);
    printf("  total:   %fms\n", totalTime);
    printf("  average: %fms\n", averageTime);
    printf("Number of clusters: %d\n\n", nOfClusters);

    // map the pixels to obtain the segmented image
    matrixPixels->map(matrixModes, clusters);

    // create the output image input
    auto output = new uint8_t[width * height * RGB_CHANNELS];
    matrixPixels->save(output);

    delete[] clusters;

    return output;
}

uint8_t* soaOmpImageSegmentationRun(uint8_t* input, int width, int height)
{
    // create the index array
    auto clusters = new int[width * height];

    // create the structures of arrays
    auto soa_pixels = std::make_shared<ImageSoa>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
    auto soa_modes = std::make_shared<ImageSoa>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);

    // initialize the pixel data
    soa_pixels->load(input);

    // clean the result variables
    int nOfClusters = 0;
    float totalTime = 0;

    // function loop
    printf("Using # %d threads.\n", omp_get_max_threads());
    for (int i = 0; i < ITERATIONS; ++i)
    {
        printf("Calling the MeanShift function... (%d)\n", i);

        // time the function
        auto start_time = high_resolution_clock::now();
        nOfClusters = soaMeanShiftOmp(soa_pixels, width * height, BANDWIDTH, soa_modes, clusters);
        auto end_time = high_resolution_clock::now();

        totalTime += (float) duration_cast<microseconds>(end_time - start_time).count() / 1000.f;
    }
    float averageTime = totalTime / ITERATIONS;

    // print the results
    printf("SoA timings: (measured on %d iterations)\n", ITERATIONS);
    printf("  total:   %fms\n", totalTime);
    printf("  average: %fms\n", averageTime);
    printf("Number of clusters: %d\n\n", nOfClusters);

    // map the pixels to obtain the segmented image
    soa_pixels->map(soa_modes, clusters);

    // create the output image input
    auto output = new uint8_t[width * height * RGB_CHANNELS];
    soa_pixels->save(output);

    delete[] clusters;

    return output;
}

int main()
{
	PPM image;
    
    // read the input image buffer
	if (image.read(INPUT_PATH) != 0) { return -1; }
	uint8_t* buffer = image.getImageHandler();

	// run the experiments
    uint8_t* matrixBuffer = matrixOmpImageSegmentationRun(buffer, image.getW(), image.getH());
    uint8_t* soaBuffer = soaOmpImageSegmentationRun(buffer, image.getW(), image.getH());

	// save the results to file
	image.load(matrixBuffer, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_AOS_OMP) != 0)	{ return 1; }
    image.load(soaBuffer, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_SOA_OMP) != 0)	{ return 2; }

	// cleanup
	delete[] matrixBuffer;
    delete[] soaBuffer;

	return 0;
}
