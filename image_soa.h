#ifndef PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H
#define PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H

#include "image.h"

class ImageSoa : public Image
{
private:
	Unique<Unique<float[]>[]>soa_;

public:
    explicit ImageSoa(int width, int height, int channels, int maxValue);
    ImageSoa(const ImageSoa& other);
    explicit ImageSoa(const Ref<ImageSoa>& other);

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


#endif //PARALLEL_MEAN_SHIFT_OPENMP_IMAGE_SOA_H
