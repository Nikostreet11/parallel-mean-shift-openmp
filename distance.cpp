#ifndef PARALLEL_MEAN_SHIFT_OPENMP_DISTANCE_CPP
#define PARALLEL_MEAN_SHIFT_OPENMP_DISTANCE_CPP

#include <cmath>

// Euclidean distance
float l2Distance(float* row1, float* row2, size_t size) {
    double distance = 0;
    for (size_t i = 0; i < size; ++i) {
        float diff = row1[i] - row2[i];
        distance += static_cast<double>(diff) * diff;
    }
	return static_cast<float>(sqrt(distance));
}

// Euclidean squared distance - faster
float l2SquaredDistance(const float* row1, const float* row2, size_t size) {
	double distance = 0;
	for (size_t i = 0; i < size; ++i) {
        float diff = row1[i] - row2[i];
		distance += static_cast<double>(diff) * diff;
	}
	return static_cast<float>(distance);
}

#endif // PARALLEL_MEAN_SHIFT_OPENMP_DISTANCE_CPP