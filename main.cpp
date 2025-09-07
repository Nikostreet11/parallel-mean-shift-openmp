#include "options.h"
#include "image_arr.h"
#include "image_aos.h"
#include "image_soa.h"
#include "experiment.cpp"

#include "include/ppm_io.cpp"

int main()
{
	PPM image;
    
    // read the input image buffer
	if (image.read(INPUT_PATH) != 0) { return -1; }
	uint8_t* buffer = image.getImageHandler();

	// create the test implementations
	auto pixelsArr = std::make_shared<ImageArr>(image.getW(), image.getH(), RGB_CHANNELS, RGB_MAX_VALUE);
	auto pixelsAos = std::make_shared<ImageAos>(image.getW(), image.getH(), RGB_CHANNELS, RGB_MAX_VALUE);
	auto pixelsSoa = std::make_shared<ImageSoa>(image.getW(), image.getH(), RGB_CHANNELS, RGB_MAX_VALUE);

	// run the experiments
	auto bufferArrSeq = imageSegmentationSeqExperiment(pixelsArr, buffer);
	auto bufferAosSeq = imageSegmentationSeqExperiment(pixelsAos, buffer);
	auto bufferSoaSeq = imageSegmentationSeqExperiment(pixelsSoa, buffer);
	auto bufferArrOmp = imageSegmentationOmpExperiment(pixelsArr, buffer);
	auto bufferAosOmp = imageSegmentationOmpExperiment(pixelsAos, buffer);
	auto bufferSoaOmp = imageSegmentationOmpExperiment(pixelsSoa, buffer);

	// save the results to file
	image.load(bufferArrSeq, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_ARR_SEQ) != 0)	{ return 1; }
	image.load(bufferAosSeq, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_AOS_SEQ) != 0)	{ return 2; }
	image.load(bufferSoaSeq, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_SOA_SEQ) != 0)	{ return 3; }
	image.load(bufferArrOmp, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_ARR_OMP) != 0)	{ return 4; }
	image.load(bufferAosOmp, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_AOS_OMP) != 0)	{ return 5; }
	image.load(bufferSoaOmp, image.getH(), image.getW(), image.getMax(), image.getMagic());
	if (image.write(OUTPUT_PATH_SOA_OMP) != 0)	{ return 6; }

	return 0;
}
