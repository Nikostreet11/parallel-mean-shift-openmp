#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_H

#include "alias.h"

#include <cstdint>

class Image {
protected:
	const int width_;
	const int height_;
	const int channels_;
	const int dimension_;
	const int maxValue_;

public:
	explicit Image(int width, int height, int channels, int maxValue);
	explicit Image(const Ref<Image>& other);

	virtual ~Image() = default;

	virtual Unique<Image> clone() const = 0;

	virtual void load(const uint8_t *buffer) const = 0;
	virtual void save(uint8_t *buffer) const = 0;
	virtual void map(const Ref<Image>& source, const int* mapper) = 0;
	virtual void write(int i, float* array) = 0;
	virtual void read(const float* array, int i) = 0;
	virtual void print(int i) const = 0;

	virtual float get(int i, int channel) const = 0;

	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	int getDimension() const;
	int getMaxValue() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_H
