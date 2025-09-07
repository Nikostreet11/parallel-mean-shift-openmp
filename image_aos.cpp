#include "image_aos.h"

#include <iostream>

ImageAos::ImageAos(int width, int height, int channels, int maxValue) :
		Image(width, height, channels, maxValue),
		aos_(std::make_unique<Unique<float[]>[]>(width_ * height_))
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		aos_[i] = std::make_unique<float[]>(dimension_);
		for (int j = 0; j < dimension_; ++j)
		{
			aos_[i][j] = 0.0f;
		}
	}
}

ImageAos::ImageAos(const ImageAos& other) :
		Image(other),
		aos_(std::make_unique<Unique<float[]>[]>(width_ * height_))
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		aos_[i] = std::make_unique<float[]>(dimension_);
		for (int j = 0; j < dimension_; ++j)
		{
			aos_[i][j] = other.aos_[i][j];
		}
	}
}

ImageAos::ImageAos(const Ref<ImageAos>& other)
		:ImageAos(*other)
{}

Unique<Image> ImageAos::clone() const
{
	return std::make_unique<ImageAos>(*this);;
}

void ImageAos::load(const uint8_t *buffer) const
{
	// normalize dividing by max-1 to map the result in the range [0,1] inclusive
	float xScale = (width_  > 1) ? 1.0f / static_cast<float>(width_ - 1) : 0.0f;
	float yScale = (height_ > 1) ? 1.0f / static_cast<float>(height_ - 1) : 0.0f;

	for (int i = 0; i < width_ * height_; ++i)
	{
		// load color channels_
		for (int j = 0; j < channels_; ++j)
		{
			int buffer_idx = i * channels_ + j;
			aos_[i][j] = static_cast<float>(buffer[buffer_idx]) / maxValue_;
		}
		// load coordinates
		int row = i / width_;
		int col = i % width_;
		aos_[i][channels_]     = static_cast<float>(col) * xScale;
		aos_[i][channels_ + 1] = static_cast<float>(row) * yScale;
	}
}

void ImageAos::save(uint8_t *buffer) const
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < channels_; ++j)
		{
			buffer[i * channels_ + j] = (uint8_t) (aos_[i][j] * maxValue_);
		}
	}
}

void ImageAos::map(const Ref<Image>& source, const int *mapper)
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < dimension_; ++j)
		{
			aos_[i][j] = source->get(mapper[i], j);
		}
	}
}

void ImageAos::write(int i, float *array)
{
	for (int j = 0; j < dimension_; ++j)
	{
		array[j] = aos_[i][j];
	}
}

void ImageAos::read(const float *array, int i)
{
	for (int j = 0; j < dimension_; ++j)
	{
		aos_[i][j] = array[j];
	}
}

void ImageAos::print(int i) const
{
	std::cout << "[ ";
	for (int j = 0; j < dimension_; ++j)
	{
		std::cout << aos_[i][j] << " ";
	}
	std::cout << "]" << std::endl;
}

float ImageAos::get(int i, int channel) const
{
	return aos_[i][channel];
}

std::string ImageAos::getImplementation() const
{
	return "Array of structures";
}
