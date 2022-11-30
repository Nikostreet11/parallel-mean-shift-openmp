#include <iostream>
#include "distance.cpp"

using namespace std;

/**
 * Cluster RGB pixels with the mean shift algorithm
 *
 * The mean shift algorithm is used in a 5-dimensional space (R, G, B, X, Y) to cluster the
 * pixels of an image.
 *
 * @param points the structure of arrays containing the pixel values
 * @param modes the resulting modes to compute
 * @param bandwidth the radius of window size to compute the mean shift
 *
 * @todo
 * @return the array of cluster indices
 */
int matrixMeanShift(float* points, size_t n, size_t dim, int* clusters, float* modes, float bandwidth)
{
	// initialization

	// stop value for the mean-shift iterations
	float epsilon = bandwidth * 0.05;

	// counter of the number of clusters
	int clustersCount = 0;

	// label all points as "not clustered"
	for (int k = 0; k < n; ++k)
	{ clusters[k] = -1; }

	for (int i = 0; i < n; ++i)
	{
		cout << "examining point " << i << endl;

		clusters[i] = -1;

		// initialize of the mean on the current point
		float mean[dim];
		for (int k = 0; k < dim; ++k)
		{ mean[k] = points[i * dim + k]; }

		// assignment to ensure the first computation
		float shift = epsilon;

		while (shift >= epsilon)
		{
			//cout << "  iterating..." << endl;

			// initialize the centroid to 0, it will accumulate points later
			float centroid[dim];
			for (int k = 0; k < dim; ++k)
			{ centroid[k] = 0; }
			// track the number of points inside the bandwidth window
			int windowPoints = 0;

			for (int j = 0; j < n; ++j)
			{
				float point[dim];
				for (int k = 0; k < dim; ++k)
				{ point[k] = points[j * dim + k]; }

				if (l2Distance(mean, point, dim) <= bandwidth)
				{
					// accumulate the point position
					for (int k = 0; k < dim; ++k) {
						// todo: multiply by the chosen kernel
						centroid[k] += point[k];
					}
					++windowPoints;
				}
			}

			//cout << "    " << windowPoints << " points examined" << endl;

			// get the centroid dividing by the number of points taken into account
			for (int k = 0; k < dim; ++k)
			{ centroid[k] /= windowPoints; }

			shift = l2Distance(mean, centroid, dim);

			//cout << "    shift = " << shift << endl;

			// update the mean
			for (int k = 0; k < dim; ++k)
			{ mean[k] = centroid[k]; }
		}

		// mean now contains the mode of the point

		cout << "    mean: [ ";
		for (int k = 0; k < dim; ++k)
		{ cout << mean[k] << " "; }
		cout << "]" << endl;

		cout << "  finding a cluster..." << endl;


		// Fixme - trova più modes anche se sembra funzionare allo stesso modo
		int j = 0;
		while (j < clustersCount && clusters[i] == -1)
		{
			// if the current mean is near to an existing mode
			if (l2Distance(mean, &modes[j * dim], dim) < bandwidth)
			{
				cout << "    cluster " << j << " similar" << endl;

				cout << "      cluster: [ ";
				for (int k = 0; k < dim; ++k)
				{ cout << modes[j * dim + k] << " "; }
				cout << "]" << endl;

				// assign the point i to the cluster j
				clusters[i] = j;
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (clusters[i] == -1) {
			cout << "    no similar cluster, create a new one (" << clustersCount << ")" << endl;

			// create a new cluster associated with the mode of the point i
			clusters[i] = clustersCount;

			for (int k = 0; k < dim; ++k)
			{ modes[clustersCount * dim + k] = mean[k]; }

			clustersCount++;
		}

		/*if (i == 0) {
			cout << "clusters: [ ";
			for (int k = 0; k < n; ++k) { cout << clusters[k] << " "; }
			cout << "]" << endl;
		}

		/*int j = 0;
		while (j < n && clusters[i] == -1)
		{
			// if the point j already belongs to a cluster, and they belong to the same mode
			if (clusters[j] != -1 && l2Distance(mean, &modes[clusters[j] * dim], dim) < bandwidth)
			{
				cout << "    cluster " << clusters[j] << " similar" << endl;
				// assign the point i to the cluster j
				clusters[i] = clusters[j];
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (j == n) {
			// create a new cluster associated with the mode of the point i
			cout << "    no similar cluster, create a new one (" << clustersCount << ")" << endl;
			clusters[i] = clustersCount;
			for (int k = 0; k < dim; ++k)
			{ modes[clustersCount * dim + k] = mean[k]; }
			clustersCount++;
		}*/
	}

	return clustersCount;
}