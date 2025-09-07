#include "image_soa.h"

#include <iostream>

ImageSoa::ImageSoa(int width, int height, int channels, int maxValue) :
		width_(width),
		height_(height),
		channels_(channels),
		dimension_(channels + 2),
		maxValue_(maxValue),
		soa_(std::make_unique<std::unique_ptr<float[]>[]>(dimension_))
{
	for (int i = 0; i < dimension_; ++i) {
		soa_[i] = std::make_unique<float[]>(width_ * height_);
		for (int j = 0; j < width_ * height_; ++j)
			soa_[i][j] = 0.0f;
	}
}

ImageSoa::ImageSoa(Ref<ImageSoa> copy) :
		width_(copy->width_),
		height_(copy->height_),
		channels_(copy->channels_),
		dimension_(copy->dimension_),
		maxValue_(copy->maxValue_),
		soa_(std::make_unique<std::unique_ptr<float[]>[]>(dimension_))
{
	for (int i = 0; i < dimension_; ++i)
	{
		soa_[i] = std::make_unique<float[]>(width_ * height_);
		for (int j = 0; j < width_ * height_; ++j)
			soa_[i][j] = copy->soa_[i][j];
	}
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

void ImageSoa::map(const Ref<ImageSoa>& source, const int *mapper)
{
	for (int i = 0; i < width_ * height_; ++i)
	{
		for (int j = 0; j < dimension_; ++j)
		{
			soa_[j][i] = (source->soa_[j][mapper[i]]);
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

void ImageSoa::print(int i)
{
	std::cout << "[ ";
	for (int j = 0; j < dimension_; ++j)
	{
		std::cout << soa_[j][i] << " ";
	}
	std::cout << "]" << std::endl;
}

int ImageSoa::getWidth() const
{
    return width_;
}

int ImageSoa::getHeight() const
{
    return height_;
}

int ImageSoa::getChannels() const
{
    return channels_;
}

int ImageSoa::getDimension() const
{
    return dimension_;
}

int ImageSoa::getMaxValue() const
{
    return maxValue_;
}
