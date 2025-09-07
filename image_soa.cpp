#include "image_soa.h"

#include <iostream>

ImageSoa::ImageSoa(int width, int height, int channels, int maxValue) :
		Image(width, height, channels, maxValue),
		soa_(std::make_unique<std::unique_ptr<float[]>[]>(dimension_))
{
	for (int i = 0; i < dimension_; ++i) {
		soa_[i] = std::make_unique<float[]>(width_ * height_);
		for (int j = 0; j < width_ * height_; ++j)
			soa_[i][j] = 0.0f;
	}
}

ImageSoa::ImageSoa(const ImageSoa& other) :
		Image(other),
		soa_(std::make_unique<std::unique_ptr<float[]>[]>(dimension_))
{
	for (int i = 0; i < dimension_; ++i)
	{
		soa_[i] = std::make_unique<float[]>(width_ * height_);
		for (int j = 0; j < width_ * height_; ++j)
			soa_[i][j] = other.soa_[i][j];
	}
}

ImageSoa::ImageSoa(const Ref<ImageSoa>& other)
		: ImageSoa(*other)
{}

Unique<Image> ImageSoa::clone() const
{
	return std::make_unique<ImageSoa>(*this);;
}

void ImageSoa::load(const uint8_t* buffer) const
{
	// normalize dividing by max-1 to map the result in the range [0,1] inclusive
	float xScale = (width_  > 1) ? 1.0f / static_cast<float>(width_  - 1) : 0.0f;
	float yScale = (height_ > 1) ? 1.0f / static_cast<float>(height_ - 1) : 0.0f;

	for (int i = 0; i < width_ * height_; ++i)
	{
		// load color channels_
		for (int j = 0; j < channels_; ++j)
		{
			int buffer_idx = i * channels_ + j;
			soa_[j][i] = static_cast<float>(buffer[buffer_idx]) / maxValue_;
		}
		// load coordinates
		int row = i / width_;
		int col = i % width_;
		soa_[channels_][i]     = static_cast<float>(col) * xScale;
		soa_[channels_ + 1][i] = static_cast<float>(row) * yScale;
	}
}

void ImageSoa::save(uint8_t *buffer) const
{
    for (int i = 0; i < width_ * height_; ++i)
    {
        for (int j = 0; j < channels_; ++j)
        {
            buffer[i * channels_ + j] = (uint8_t) (soa_[j][i] * maxValue_);
        }
    }
}

void ImageSoa::map(const Ref<Image>& source, const int *mapper)
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < dimension_; ++j)
		{
			soa_[j][i] = source->get(mapper[i], j);
		}
	}
}

void ImageSoa::write(int i, float *array)
{
	for (int j = 0; j < dimension_; ++j)
	{
		array[j] = soa_[j][i];
	}
}

void ImageSoa::read(const float *array, int i)
{
	for (int j = 0; j < dimension_; ++j)
	{
		soa_[j][i] = array[j];
	}
}

void ImageSoa::print(int i) const
{
	std::cout << "[ ";
	for (int j = 0; j < dimension_; ++j)
	{
		std::cout << soa_[j][i] << " ";
	}
	std::cout << "]" << std::endl;
}

float ImageSoa::get(int i, int channel) const
{
	return soa_[channel][i];
}
