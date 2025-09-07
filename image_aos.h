#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_AOS_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_AOS_H

#include "image.h"

class ImageAos : public Image
{
private:
	Unique<Unique<float[]>[]>aos_;

public:
	explicit ImageAos(int width, int height, int channels, int maxValue);
	ImageAos(const ImageAos& other);
	explicit ImageAos(const Ref<ImageAos>& other);

	Unique<Image> clone() const override;

	void load(const uint8_t *buffer) const override;
	void save(uint8_t *buffer) const override;
	void map(const Ref<Image>& source, const int* mapper) override;
	void write(int i, float* array) override;
	void read(const float* array, int i) override;
	void print(int i) const override;

	float get(int i, int channel) const override;
	std::string getImplementation() const override;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_AOS_H
