#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H

#include <cstdint>
#include "alias.h"

class ImageMatrix
{
private:
	const int width_;
	const int height_;
	const int channels_;
	const int dimension_;
	const int maxValue_;
	Unique<float[]> arr_;

public:
	explicit ImageMatrix(int width, int height, int channels, int maxValue);
	explicit ImageMatrix(Ref<ImageMatrix> copy);

    void load(const uint8_t *buffer) const;
    void save(uint8_t *buffer) const;
    void map(const Ref<ImageMatrix>& source, const int* mapper);
    void write(int i, float* array);
    void read(const float* array, int i);
    void print(int i);

    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    int getDimension() const;
    int getMaxValue() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
