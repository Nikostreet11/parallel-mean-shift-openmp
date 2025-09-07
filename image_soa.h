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
    explicit ImageSoa(int width, int height, int channels, int maxValue);
    explicit ImageSoa(Ref<ImageSoa> copy);

    void load(const uint8_t *buffer) const;
    void save(uint8_t *buffer) const;
    void map(const Ref<ImageSoa>& source, const int* mapper);
	void write(int i, float* array);
	void read(const float* array, int i);
	void print(int i);

    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    int getDimension() const;
    int getMaxValue() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H
