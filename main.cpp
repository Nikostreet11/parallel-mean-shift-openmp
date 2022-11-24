#include <iostream>
#include "libs/ppm_io.h"
#include <cmath>
#define DIMENSION 5;
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

// Euclidean distance function
float l2Distance(float* row1, float* row2, size_t size) {
	float distance = 0;
	for (int i = 0; i < size; ++i) {
		distance += std::pow(row1[i] - row2[i], 2);
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
    // bandwith è l'iperparametro usato come soglia per prendere i punti da usare nel calcolo della media

	// sanity check
	if (&points == &modes) {
		printf("Error - Pixel and modes can't be the same structure!");
		return NULL;
	}

	// initialize the stop value for the mean computing
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

int meanShiftTest(float* points, size_t n, size_t dim, int* clusters, float* modes, float bandwidth)
{
	// initialization
	float epsilon = bandwidth * 0.01;
	// epsilon usata per stoppare calcolo della media se il punto non si è spostato più di essa
	int clustersCount = 0;

	for (int i = 0; i < n; ++i)
	{
	    if(i%10==0){
	        cout<<"Procedo..."<<endl;
	    }

		// label the point as "not clustered"
		clusters[i] = -1;

		// initialize of the mean on the current point
		float mean[dim];
		for (int k = 0; k < dim; ++k)
		{ mean[k] = points[i * dim + k]; }

		// assignment to ensure the first computation
		float shift = epsilon;

		while (shift >= epsilon)
		{
		    cout<<"ripeto per pixel "<< i<<endl;

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
		while (j<n && clusters[i] == -1)
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

	cout <<"Ho finito!!!!"<<endl;

	return clustersCount;
}

// todo: convert from RGB to XYZ to L*U*V*
// todo: convert from RGB to HSV (prof advice)

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

    /*
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


    delete[] Pixels.R;
    delete[] Pixels.G;
    delete[] Pixels.B;
    delete[] Pixels.X;
    delete[] Pixels.Y;

    */

    float points[width*height*5];

    int j=0;
    for(int i=0; i<width*height*3; i=i+3){
        points[j]=(float)buffer_image[i]/255;
        points[j+1]=(float)buffer_image[i+1]/255;
        points[j+2]=(float)buffer_image[i+2]/255;
        points[j+3]=(float)((i/3)%width)/(width-1);
        points[j+4]=(float)((i/3)/width)/(height-1);
        j+=5;
    }

    float modes[width*height*5];
    int clusters[width*height];
    int dimension = 5;

    cout<<"Chiamo la funzione meanShiftTest"<<endl;
    int numOfClusters= meanShiftTest(points,width*height,dimension,clusters,modes,0.3);

    uint8_t buffer_image_new[width*height*3];

    int k=0;
    for(int i=0; i<width*height*3; i+=3){
        buffer_image_new[i]=(uint8_t)(modes[clusters[k]*dimension]*255);
        buffer_image_new[i+1]=(uint8_t)(modes[clusters[k]*dimension+1]*255);
        buffer_image_new[i+2]=(uint8_t)(modes[clusters[k]*dimension+2]*255);
        k++;
    }

    ppmIn.load(buffer_image_new,height,width,ppmIn.getMax(),ppmIn.getMagic());
    std::string outFilepath = "../Image/image_out.ppm";
    status = ppmIn.write(outFilepath);
    cout <<status<<endl;
    cout <<"Nuova immagine salvata"<<endl;
    cout <<"Numero di Cluster: "<<numOfClusters<<endl;





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
