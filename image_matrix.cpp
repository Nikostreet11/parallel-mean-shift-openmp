#include "image_matrix.h"

ImageMatrix::ImageMatrix(int w, int h, int c, int max) :
		width(w),
		height(h),
		channels(c),
		dimension(c + 2),
		maxValue(max),
		matrix(std::make_unique<float[]>(w * h * (dimension)))
{
	for (int i = 0; i < width * height * dimension; ++i)
		matrix[i] = 0.0f;
}

void ImageMatrix::load(const uint8_t *buffer) const
{
	// normalize dividing by max-1 to map the result in the range [0,1] inclusive
	float xScale = (width  > 1) ? 1.0f / static_cast<float>(width  - 1) : 0.0f;
	float yScale = (height > 1) ? 1.0f / static_cast<float>(height - 1) : 0.0f;

	for (int i = 0; i < width * height; ++i)
	{
		// load color channels
		for (int j = 0; j < channels; ++j)
		{
			int pixels_idx = i * dimension + j;
			int buffer_idx = i * channels + j;
			matrix[pixels_idx] = static_cast<float>(buffer[buffer_idx]) / maxValue;
		}
		// load coordinates
		int row = i / width;
		int col = i % width;
		matrix[i * dimension + channels]     = static_cast<float>(col) * xScale;
		matrix[i * dimension + channels + 1] = static_cast<float>(row) * yScale;
	}
}

float *ImageMatrix::getPixels() // NOLINT
{
	return matrix.get();
}

const float *ImageMatrix::getPixels() const
{
	return matrix.get();
}

int ImageMatrix::getWidth() const
{
	return width;
}

int ImageMatrix::getHeight() const
{
	return height;
}

int ImageMatrix::getChannels() const
{
	return channels;
}

int ImageMatrix::getDimension() const {
	return dimension;
}

int ImageMatrix::getMaxValue() const
{
	return getMaxValue();
}
