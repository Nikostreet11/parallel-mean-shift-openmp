#ifndef MEANSHIFT_OMP_CPP
#define MEANSHIFT_OMP_CPP

#define CHANNELS 5

#include "image.h"
#include "distance.cpp"

int meanShiftOmp(const Ref<Image>& points, float bandwidth, const Ref<Image>& modes, int* clusters)
{
	// sanity check
	if (points == modes)
	{
		printf("Error - Pixel and modes can't be the same object!");
		return -1;
	}

	auto squaredBandwidth = (float) pow(bandwidth, 2);

	// stop value to check for the shift convergence
	auto epsilon = (float) pow(bandwidth * 0.05, 2);

	// structure to save the final mean of each pixel
	auto means = points->clone();

	// compute the means
	#pragma omp parallel default(none) shared(points, means, modes) firstprivate(epsilon, squaredBandwidth)
	{
		#pragma omp for
		for (int i = 0; i < points->getSize(); ++i)
		{
			// initialize the mean on the current point
			float mean[CHANNELS];
			points->write(i, mean);

			// assignment to ensure the first computation
			float shift = epsilon;

			while (shift >= epsilon)
			{
				// initialize the centroid to 0, it will accumulate points later
				float centroid[CHANNELS];
				for (float& k : centroid) { k = 0; }

				// track the number of points inside the bandwidth window
				int windowPoints = 0;

				for (int j = 0; j < points->getSize(); ++j)
				{
					float point[CHANNELS];
					points->write(j, point);

					if (l2SquaredDistance(mean, point, CHANNELS) <= squaredBandwidth)
					{
						// accumulate the point position
						for (int k = 0; k < CHANNELS; ++k)
						{
							centroid[k] += point[k];
						}
						++windowPoints;
					}
				}

				// get the centroid dividing by the number of points taken into account
				for (float& k : centroid) { k /= (float) windowPoints; }

				shift = l2SquaredDistance(mean, centroid, CHANNELS);

				// update the mean
				for (int k = 0; k < CHANNELS; ++k) { mean[k] = centroid[k]; }
			}

			// mean now contains the mode of the point
			means->read(mean, i);
		}
	}

	// label all points as "not clustered"
	for (int k = 0; k < points->getSize(); ++k) { clusters[k] = -1; }

	// counter for the number of discovered clusters
	int clustersCount = 0;

	for (int i = 0; i < points->getSize(); ++i)
	{
		float mean[CHANNELS];
		means->write(i, mean);

		int j = 0;
		while (j < clustersCount && clusters[i] == -1)
		{
			// select the current mode
			float mode[CHANNELS];
			modes->write(j, mode);

			// if the mean is close enough to the current mode
			if(l2SquaredDistance(mean, mode, CHANNELS) < squaredBandwidth)
			{
				// assign the point i to the cluster j
				clusters[i] = j;
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (clusters[i] == -1)
		{
			// create a new cluster associated with the mode of the point i
			clusters[i] = clustersCount;

			modes->read(mean, clustersCount);

			clustersCount++;
		}
	}

	return clustersCount;
}

#endif // MEANSHIFT_OMP_CPP