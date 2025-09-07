#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H

#include "image.h"

class ImageMatrix : public Image
{
private:
	Unique<float[]> arr_;

public:
	explicit ImageMatrix(int width, int height, int channels, int maxValue);
	ImageMatrix(const ImageMatrix& other);
	explicit ImageMatrix(const Ref<ImageMatrix>& other);

	Unique<Image> clone() const override;

	void load(const uint8_t *buffer) const override;
	void save(uint8_t *buffer) const override;
	void map(const Ref<Image>& source, const int* mapper) override;
	void write(int i, float* array) override;
	void read(const float* array, int i) override;
	void print(int i) const override;

	float get(int i, int channel) const override;
};


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_MATRIX_H
