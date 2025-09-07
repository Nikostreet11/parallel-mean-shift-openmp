#include "image_matrix.h"

#include <iostream>

ImageMatrix::ImageMatrix(int width, int height, int channels, int maxValue) :
        width_(width),
        height_(height),
        channels_(channels),
        dimension_(channels + 2),
        maxValue_(maxValue),
        arr_(std::make_unique<float[]>(width * height * dimension_))
{
	for (int i = 0; i < width_ * height_ * dimension_; ++i)
	{
		arr_[i] = 0.0f;
	}
}

ImageMatrix::ImageMatrix(Ref<ImageMatrix> copy) :
		width_(copy->width_),
		height_(copy->height_),
		channels_(copy->channels_),
		dimension_(copy->dimension_),
		maxValue_(copy->maxValue_),
		arr_(std::make_unique<float[]>(width_ * height_ * dimension_))
{
	for (int i = 0; i < width_ * height_ * dimension_; ++i)
	{
		arr_[i] = copy->arr_[i];
	}
}

void ImageMatrix::load(const uint8_t *buffer) const
{
	// normalize dividing by max-1 to map the result in the range [0,1] inclusive
	float xScale = (width_ > 1) ? 1.0f / static_cast<float>(width_ - 1) : 0.0f;
	float yScale = (height_ > 1) ? 1.0f / static_cast<float>(height_ - 1) : 0.0f;

	for (int i = 0; i < width_ * height_; ++i)
	{
		// load color channels_
		for (int j = 0; j < channels_; ++j)
		{
			int pixels_idx = i * dimension_ + j;
			int buffer_idx = i * channels_ + j;
            arr_[pixels_idx] = static_cast<float>(buffer[buffer_idx]) / maxValue_;
		}
		// load coordinates
		int row = i / width_;
		int col = i % width_;
        arr_[i * dimension_ + channels_]     = static_cast<float>(col) * xScale;
        arr_[i * dimension_ + channels_ + 1] = static_cast<float>(row) * yScale;
	}
}

void ImageMatrix::save(uint8_t *buffer) const
{
    for (int i = 0; i < width_ * height_; ++i)
    {
        for (int j = 0; j < channels_; ++j)
        {
            buffer[i * channels_ + j] = (uint8_t) (arr_[i * dimension_ + j] * maxValue_);
        }
    }
}

void ImageMatrix::map(const Ref<ImageMatrix> &source, const int *mapper)
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < dimension_; ++j)
		{
			arr_[i * dimension_ + j] = (source->arr_[mapper[i] * dimension_ + j]);
		}
	}
}

void ImageMatrix::write(int i, float *array)
{
	for (int j = 0; j < dimension_; ++j)
	{
		array[j] = arr_[i * dimension_ + j];
	}
}

void ImageMatrix::read(const float *array, int i)
{
	for (int j = 0; j < dimension_; ++j)
	{
		arr_[i * dimension_ + j] = array[j];
	}
}

void ImageMatrix::print(int i)
{
	std::cout << "[ ";
	for (int j = 0; j < dimension_; ++j)
	{
		std::cout << arr_[i * dimension_ + j] << " ";
	}
	std::cout << "]" << std::endl;
}

int ImageMatrix::getWidth() const
{
	return width_;
}

int ImageMatrix::getHeight() const
{
	return height_;
}

int ImageMatrix::getChannels() const
{
	return channels_;
}

int ImageMatrix::getDimension() const {
	return dimension_;
}

int ImageMatrix::getMaxValue() const
{
	return maxValue_;
}
