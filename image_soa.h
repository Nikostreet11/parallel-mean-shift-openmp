#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H

#include <cstdint>
#include "alias.h"

class ImageSoa
{
private:
	const int width_;
	const int height_;
	const int channels_;
	const int dimension_;
	const int maxValue_;
	Unique<Unique<float[]>[]>soa_;

public:
	explicit ImageSoa(int width, int height, int channels = 3, int maxValue = 255);

    void load(const uint8_t *_buffer) const;
    void save(uint8_t *_buffer) const;
    void map(const Ref<ImageSoa>& source, const int* mapper);
	void write(int i, float* array);
	void read(const float* array, int i);
	void print(int i);

    const int getWidth() const;
    const int getHeight() const;
    const int getChannels() const;
    const int getDimension() const;
    const int getMaxValue() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H
