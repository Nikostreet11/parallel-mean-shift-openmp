#include "image_matrix.h"

#include <iostream>

ImageMatrix::ImageMatrix(int width, int height, int channels, int maxValue) :
		Image(width, height, channels, maxValue),
        arr_(std::make_unique<float[]>(width * height * dimension_))
{
	for (int i = 0; i < width_ * height_ * dimension_; ++i)
	{
		arr_[i] = 0.0f;
	}
}

ImageMatrix::ImageMatrix(const ImageMatrix& other) :
		Image(other),
		arr_(std::make_unique<float[]>(width_ * height_ * dimension_))
{
	for (int i = 0; i < width_ * height_ * dimension_; ++i)
	{
		arr_[i] = other.arr_[i];
	}
}

ImageMatrix::ImageMatrix(const Ref<ImageMatrix>& other)
		: ImageMatrix(*other)
{}

Unique<Image> ImageMatrix::clone() const
{
	return std::make_unique<ImageMatrix>(*this);;
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

void ImageMatrix::map(const Ref<Image>& source, const int *mapper)
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < dimension_; ++j)
		{
			arr_[i * dimension_ + j] = (source->get(mapper[i], j));
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

void ImageMatrix::print(int i) const
{
	std::cout << "[ ";
	for (int j = 0; j < dimension_; ++j)
	{
		std::cout << arr_[i * dimension_ + j] << " ";
	}
	std::cout << "]" << std::endl;
}

float ImageMatrix::get(int i, int channel) const
{
	return arr_[i * dimension_ + channel];
}
