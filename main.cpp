#include <iostream>
#include "libs/ppm_io.h"
#include <cmath>

using namespace std;

// structure of arrays
struct pixels{
    float* R;
    float* G;
    float* B;
    float* X;
    float* Y;
};

float distance(float r1, float g1, float b1, float x1, float y1, float r2, float g2, float b2, float x2, float y2){
    return sqrt(pow((r1-r2),2)+pow((g1-g2),2)+pow((b1-b2),2)+pow((x1-x2),2)+pow((y1-y2),2));
}

// euclidean distance function
float l2Distance(float* x, float* y, size_t size) {
	float distance = 0;
	for (int i = 0; i < size; ++i) {
		distance += std::pow(x[i] + y[i], 2);
	}
	return sqrt(distance);
}

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
float* meanShift(pixels &points, size_t nOfPoints, pixels &modes, float bandwidth)
{
	// sanity check
	if (&points == &modes) {
		printf("Error - Pixel and modes can't be the same structure!");
		return NULL;
	}

	// initialize the stop value
	float epsilon = bandwidth / 100;

	/*// initialize the centroids to the initial position of each point
	for (size_t i = 0; i < nOfPoints; ++i) {
		modes.R[i] = points.R[i];
		modes.G[i] = points.G[i];
		modes.B[i] = points.B[i];
		modes.X[i] = points.X[i];
		modes.Y[i] = points.Y[i];
	}*/

    for (int i = 0; i < nOfPoints; ++i)
	{
		// initialization of the mean on the point position
		float mean[5] = {points.R[i],
						 points.G[i],
						 points.B[i],
						 points.X[i],
						 points.Y[i]};

		// assignment to ensure the first computation
		float shift = epsilon;

		while (shift > epsilon) {
			float centroid[5] {0, 0, 0, 0, 0};

			// centroid <- mean
//			copy(begin(mean), end(mean), begin(centroid));

			// track the number of points inside the bandwidth window
			int windowPoints = 0;

			// for every other point
			for (int j = 0; j < nOfPoints; ++j) {

				// initialize the current sample
				float point[5] = {points.R[j],
								  points.G[j],
								  points.B[j],
								  points.X[j],
								  points.Y[j]};

				if (l2Distance(mean, point, 5) < bandwidth) {
					// accumulate the point position
					for (int k = 0; k < 5; ++k) {
						// todo: multiply by the chosen kernel
						centroid[k] += point[k];
					}
					++windowPoints;
				}
			}
			// divide by the number of points taken into account
			for (int k = 0; k < 5; ++k) {
				centroid[k] /= windowPoints;
			}

			shift = l2Distance(mean, centroid, 5);

			// update the mean (mean <- centroid)
			copy(begin(centroid), end(centroid), begin(mean));
		}

		// assign the resulting mean to the modes structure
		modes.R[i] = mean[0];
		modes.G[i] = mean[1];
		modes.B[i] = mean[2];
		modes.X[i] = mean[3];
		modes.Y[i] = mean[4];
	}

    return NULL;
}

float* meanShiftTest(float* points, size_t n, size_t dim, int* clusters, float bandwidth)
{
	// initialization
	float* modes = new float[n * dim];
	float epsilon = bandwidth * 0.01;
	int clustersCount = 0;

	for (int i = 0; i < n; ++i)
	{
		// label the point as "not clustered"
		clusters[i] = -1;

		// initialize of the mean on the point position
		float mean[dim];
		for (int k = 0; k < dim; ++k)
		{ mean[k] = points[i * dim + k]; }

		// assignment to ensure the first computation
		float shift = epsilon;

		while (shift > epsilon)
		{
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

				if (l2Distance(mean, point, dim) < bandwidth)
				{
					// accumulate the point position
					for (int k = 0; k < dim; ++k) {
						// todo: multiply by the chosen kernel
						centroid[k] += point[k];
					}
					++windowPoints;
				}
			}

			// get the centroid dividing by the number of points taken into account
			for (int k = 0; k < dim; ++k)
			{ centroid[k] /= windowPoints; }

			shift = l2Distance(mean, centroid, dim);

			// update the mean
			for (int k = 0; k < dim; ++k)
			{ mean[k] = centroid[k]; }
		}

		// mean now contains the mode of the point

		int j = 0;
		while (clusters[i] == -1)
		{
			// if the point j already belongs to a cluster, and they belong to the same mode
			if (clusters[j] != -1 && l2Distance(mean, &modes[clusters[j] * dim], dim) < bandwidth)
			{
				// assign the point i to the cluster j
				clusters[i] = clusters[j];
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (j == n) {
			// create a new cluster associated with the mode of the point i
			clusters[i] = clustersCount;
			for (int k = 0; k < dim; ++k)
			{ modes[clustersCount * dim + k] = mean[k]; }
			clustersCount++;
		}
	}

	return modes;
}

// todo: convert from RGB to XYZ to L*U*V*

int main()
{
    // OPEN IMAGE PPM
    PPM ppmIn;
    int status;
    std::string inFilepath = "../Image/image.ppm";
    status = ppmIn.read(inFilepath);
    cout <<status<<endl;
    int width= ppmIn.getW();
    int height= ppmIn.getH();
    uint8_t* buffer_image= ppmIn.getImageHandler();

    // CREATION OF 5 ARRAY R, G, B, X, Y
    pixels Pixels;
    Pixels.R= new float[width*height];
    Pixels.G= new float[width*height];
    Pixels.B= new float[width*height];
    Pixels.X= new float[width*height];
    Pixels.Y= new float[width*height];

    int j=0;
    for(int i=0; i<width*height*3; i++){
        if(i%3==0){
            Pixels.R[j]=(float)buffer_image[i]/255;
        } else if (i%3==1){
            Pixels.G[j]=(float)buffer_image[i]/255;
        } else{
            Pixels.B[j]=(float)buffer_image[i]/255;
            Pixels.X[j]=(float)((i/3)%width)/(width-1);
            Pixels.Y[j]=(float)((i/3)/width)/(height-1);
            j++;
        }
    }

    cout <<"R: ";
    for(int i=0; i<width*height; i++){
        cout <<Pixels.X[i]<<" ";
    }
    cout <<endl;

    float* Modes = new float[width*height];

    delete[] Pixels.R;
    delete[] Pixels.G;
    delete[] Pixels.B;
    delete[] Pixels.X;
    delete[] Pixels.Y;

    /*
	size_t nOfPoints = 100;
	size_t dimension = 5;

	// data initialization
	// TODO a bit clunky, use struct/2D vector instead?
	srand((unsigned) time(NULL));
	float** points = new float*[nOfPoints];
	for (int i = 0; i < nOfPoints; ++i)
	{
		points[i] = new float[dimension];
		for (int j = 0; j < dimension; ++j)
			points[i][j] = (float) rand()/RAND_MAX * 1000;
	}

	meanShift(points, nOfPoints, dimension);

	// deallocate memory
	for (int i = 0; i < nOfPoints; ++i) {
		delete[] points[i];
	}
	delete[] points;

	return 0;
     */
}
