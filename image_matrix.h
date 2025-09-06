#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H

#include <cstdint>
#include <memory>

class ImageMatrix
{
private:
	static const int MAX_VALUE = 255;
	const int width;
	const int height;
	const int channels;
	const int dimension;
	const int maxValue;
	std::unique_ptr<float[]> matrix;

public:
	ImageMatrix(int w, int h, int c = 3, int max = 255);
	void load(const uint8_t* buffer) const;

	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	int getDimension() const;
	int getMaxValue() const;

	//void print(int i);

	float* getPixels();
	const float* getPixels() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
