#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#ifndef SOA_MEANSHIFT_OMP_CPP
#define SOA_MEANSHIFT_OMP_CPP

#include "rgb_pixels.cpp"
#include "distance.cpp"
#include <omp.h>

/**
 * Cluster RGB RgbPixels with the mean shift algorithm
 *
 * The mean shift algorithm is used in a 5-dimensional space (r, g, b, x, y) to cluster the
 * RgbPixels of an image.
 *
 * @param points the structure of arrays containing the pixel values
 * @param modes the resulting modes to compute
 * @param bandwidth the radius of window size to compute the mean shift
 *
 * @todo
 * @return the array of cluster indices
 */
int soaMeanShiftOmp(RgbPixels &points, size_t n, RgbPixels &modes, int* clusters, float bandwidth) {

	std::cout << "Using # " << omp_get_max_threads() << " threads." << std::endl;

	// bandwith è l'iperparametro usato come soglia per prendere i punti da usare nel calcolo della media

	// sanity check
	if (&points == &modes) {
		printf("Error - Pixel and modes can't be the same structure!");
		return -1;
	}

	// initialization
	float epsilon = bandwidth * 0.05;
	// epsilon usata per stoppare calcolo della media se il punto non si è spostato più di essa
	int clustersCount = 0;
	RgbPixels means;
	means.create(points.width, points.height);

	// label all points as "not clustered"
	for (int k = 0; k < n; ++k) { clusters[k] = -1; }

	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < n; ++i) {
			//cout << "examining point " << i << endl;

			// initialize of the mean on the current point
			float mean[5];
			points.write(i, mean);

			// assignment to ensure the first computation
			float shift = epsilon;

			while (shift >= epsilon) {
				//cout << "  iterating..." << endl;

				// initialize the centroid to 0, it will accumulate points later
				float centroid[5];
				for (int k = 0; k < 5; ++k) { centroid[k] = 0; }

				// track the number of points inside the bandwidth window
				int windowPoints = 0;

				for (int j = 0; j < n; ++j) {
					/*float point[5]={points.r[j],
									points.g[j],
									points.b[j],
									points.x[j],
									points.y[j]};*/
					float point[5];
					points.write(j, point);

					if (l2Distance(mean, point, 5) <= bandwidth) {
						// accumulate the point position
						for (int k = 0; k < 5; ++k) {
							// todo: multiply by the chosen kernel
							centroid[k] += point[k];
						}
						++windowPoints;
					}
				}

				//cout << "    " << windowPoints << " points examined" << endl;

				// get the centroid dividing by the number of points taken into account
				for (int k = 0; k < 5; ++k) { centroid[k] /= windowPoints; }

				shift = l2Distance(mean, centroid, 5);

				//cout << "    shift = " << shift << endl;

				// update the mean
				for (int k = 0; k < 5; ++k) { mean[k] = centroid[k]; }
			}
			// mean now contains the mode of the point
			means.save(mean, i);
		}
	}

	for (int i = 0; i < n; ++i) {
		float mean[5];
		means.write(i, mean);

		/*cout << "    mean: [ ";
		for (int k = 0; k < 5; ++k)
		{ cout << mean[k] << " "; }
		cout << "]" << endl;*/

		//cout << "  finding a cluster..." << endl;

		int j = 0;
		while (j < clustersCount && clusters[i] == -1)
		{
			float mode[5];
			/*float mode[5] = {modes.r[clusters[j]],
							 modes.g[clusters[j]],
							 modes.b[clusters[j]],
							 modes.x[clusters[j]],
							 modes.y[clusters[j]]};*/
			modes.write(j, mode);

			if(l2Distance(mean, mode, 5) < bandwidth)
			{
				//cout << "    cluster " << j << " similar" << endl;

				// assign the point i to the cluster j
				clusters[i] = j;
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (clusters[i] == -1) {
			//cout << "    no similar cluster, create a new one (" << clustersCount << ")" << endl;

			// create a new cluster associated with the mode of the point i
			clusters[i] = clustersCount;

			modes.r[clustersCount]=mean[0];
			modes.g[clustersCount]=mean[1];
			modes.b[clustersCount]=mean[2];
			modes.x[clustersCount]=mean[3];
			modes.y[clustersCount]=mean[4];

			clustersCount++;
		}
	}

	return clustersCount;
}

#endif // SOA_MEANSHIFT_OMP_CPP
#pragma clang diagnostic pop