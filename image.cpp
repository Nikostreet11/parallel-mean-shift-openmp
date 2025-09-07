#include "image.h"

Image::Image(int width, int height, int channels, int maxValue)
		: width_(width),
		  height_(height),
		  channels_(channels),
		  dimension_(channels + 2),
		  maxValue_(maxValue) {}

Image::Image(const Ref<Image>& other)
		: width_(other->width_),
		  height_(other->height_),
		  channels_(other->channels_),
		  dimension_(other->dimension_),
		  maxValue_(other->maxValue_) {}

int Image::getWidth() const { return width_; }
int Image::getHeight() const { return height_; }
int Image::getSize() const { return width_ * height_; }
int Image::getChannels() const { return channels_; }
int Image::getDimension() const { return dimension_; }
int Image::getMaxValue() const { return maxValue_; }
