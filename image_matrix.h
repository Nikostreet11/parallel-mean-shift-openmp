#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H

#include <cstdint>
#include <memory>

class ImageMatrix
{
private:
	static const int MAX_VALUE = 255;
	int width;
	int height;
	int channels;
	int dimension;

public:
	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	int getDimension() const;
	std::unique_ptr<float[]> pixels;
public:

public:
	ImageMatrix(int w, int h, int c);
	void load(const uint8_t* buffer) const;
	//void print(int i);

	float* getPixels();
	const float* getPixels() const;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
