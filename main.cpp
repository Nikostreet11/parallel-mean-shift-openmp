#include "options.h"

#include "image_matrix.h"
#include "image_soa.h"
//#include "matrix_meanshift.cpp"
#include "matrix_meanshift_omp.cpp"
//#include "soa_meanshift.cpp"
#include "soa_meanshift_omp.cpp"
#include "benchmark.cpp"

#include "include/ppm_io.cpp"

#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std::chrono;

// TODO: kernel multiplication

uint8_t* matrixOmpImageSegmentationRun(uint8_t* input, int width, int height)
{
    // create the matrices
    auto pixels = std::make_shared<ImageMatrix>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
    auto modes =  std::make_shared<ImageMatrix>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
	auto clusters = std::make_unique<int[]>(width * height);

    // initialize the pixel data
    pixels->load(input);

	benchmark("MatrixMeanShift", [&]() {
		return matrixMeanShiftOmp(pixels, width * height, BANDWIDTH, modes, clusters.get());
	}, ITERATIONS);

    // map the pixels to obtain the segmented image
    pixels->map(modes, clusters.get());

    // create the output image input
    auto output = new uint8_t[width * height * RGB_CHANNELS];
    pixels->save(output);

    return output;
}

uint8_t* soaOmpImageSegmentationRun(uint8_t* input, int width, int height)
{
    // create the index array
    //auto clusters = new int[width * height];

    // create the structures of arrays
    auto pixels = std::make_shared<ImageSoa>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
    auto modes = std::make_shared<ImageSoa>(width, height, RGB_CHANNELS, RGB_MAX_VALUE);
	auto clusters = std::make_unique<int[]>(width * height);

    // initialize the pixel data
    pixels->load(input);

	benchmark("SoaMeanShift", [&]() {
		return soaMeanShiftOmp(pixels, width * height, BANDWIDTH, modes, clusters.get());
	}, ITERATIONS);

    // map the pixels to obtain the segmented image
    pixels->map(modes, clusters.get());

    // create the output image input
    auto output = new uint8_t[width * height * RGB_CHANNELS];
    pixels->save(output);

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
