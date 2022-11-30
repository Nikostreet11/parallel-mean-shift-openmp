#include <iostream>
#include "libs/ppm_io.h"
#include <cmath>
#include <chrono>

#define DIMENSION 5;
using namespace std;
using namespace chrono;

// structure of arrays
struct pixels{
    float* R;
    float* G;
    float* B;
    float* X;
    float* Y;
};

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

int meanShift(pixels &points, size_t n, pixels &modes, int* clusters, float bandwidth)
{
    // bandwith è l'iperparametro usato come soglia per prendere i punti da usare nel calcolo della media

	// sanity check
	if (&points == &modes) {
		printf("Error - Pixel and modes can't be the same structure!");
		return -1;
	}

    // initialization
    float epsilon = bandwidth * 0.005;
    // epsilon usata per stoppare calcolo della media se il punto non si è spostato più di essa
    int clustersCount = 0;

    for (int i = 0; i < n; ++i) {
        /*
        if(i%10==0){
            cout<<"Procedo..."<<endl;
        }
        */

        // label the point as "not clustered"
        clusters[i] = -1;

        // initialize of the mean on the current point
        float mean[5] = {points.R[i],
                         points.G[i],
                         points.B[i],
                         points.X[i],
                         points.Y[i]};

        // assignment to ensure the first computation
        float shift = epsilon;

        while (shift >= epsilon) {
            //cout<<"ripeto per pixel "<< i<<endl;

            // initialize the centroid to 0, it will accumulate points later
            float centroid[5];
            for (int k = 0; k < 5; ++k) { centroid[k] = 0; }
            // track the number of points inside the bandwidth window
            int windowPoints = 0;

            for (int j = 0; j < n; ++j) {
                float point[5]={points.R[j],
                               points.G[j],
                               points.B[j],
                               points.X[j],
                               points.Y[j]};


                if (l2Distance(mean, point, 5) <= bandwidth) {
                    // accumulate the point position
                    for (int k = 0; k < 5; ++k) {
                        // todo: multiply by the chosen kernel
                        centroid[k] += point[k];
                    }
                    ++windowPoints;
                }
            }

            // get the centroid dividing by the number of points taken into account
            for (int k = 0; k < 5; ++k) { centroid[k] /= windowPoints; }

            shift = l2Distance(mean, centroid, 5);

            // update the mean
            for (int k = 0; k < 5; ++k) { mean[k] = centroid[k]; }
        }

        for (int k = 0; k < 5; ++k) { cout<<mean[k]<<", ";}

        int j = 0;
        while (j<n && clusters[i] == -1)
        {
            // if the point j already belongs to a cluster, and they belong to the same mode
            if (clusters[j]!=-1)
            {
                // fixme error
                cout<<"dentro"<<endl;
                float mode[5] = {modes.R[clusters[j]],
                                 modes.G[clusters[j]],
                                 modes.B[clusters[j]],
                                 modes.X[clusters[j]],
                                 modes.Y[clusters[j]]};
                cout<<"dentro2"<<endl;
                if(l2Distance(mean, mode, 5) < bandwidth){
                    // assign the point i to the cluster j
                    clusters[i] = clusters[j];
                    cout<<clusters[j]<<endl;
                }
            }
            ++j;
        }

        // if the point i was not assigned to a cluster
        if (j == n) {
            // create a new cluster associated with the mode of the point i
            clusters[i] = clustersCount;
            cout<<clustersCount<<endl;
            modes.R[clustersCount]=mean[0];
            modes.G[clustersCount]=mean[1];
            modes.B[clustersCount]=mean[2];
            modes.X[clustersCount]=mean[3];
            modes.Y[clustersCount]=mean[4];
            clustersCount++;
        }
    }

    return clustersCount;
}


int meanShiftTest(float* points, size_t n, size_t dim, int* clusters, float* modes, float bandwidth)
{
	// initialization
	float epsilon = bandwidth * 0.005;
	// epsilon usata per stoppare calcolo della media se il punto non si è spostato più di essa
	int clustersCount = 0;

	for (int i = 0; i < n; ++i)
	{
	    /*
	    if(i%10==0){
	        cout<<"Procedo..."<<endl;
	    }
	    */

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
		    //cout<<"ripeto per pixel "<< i<<endl;

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
        /*
        // Fixme - trova più modes anche se sembra funzionare allo stesso modo
		int j = 0;
		while (j<clustersCount && clusters[i] == -1)
		{
			// if the current mean of the point is enought similiar to a mode already existent
			if (l2Distance(mean, &modes[j * dim], dim) < bandwidth)
			{
				// assign the point i to the cluster j
				clusters[i] = j;
			}
			++j;
		}

		// if the point i was not assigned to a cluster
		if (j == clustersCount) {
			// create a new cluster associated with the mode of the point i
			clusters[i] = clustersCount;
            cout<<clustersCount<<endl;
			for (int k = 0; k < dim; ++k)
			{ modes[clustersCount * dim + k] = mean[k]; }
			clustersCount++;
		}
		*/

        int j = 0;
        while (j<n && clusters[i] == -1)
        {
            // if the point j already belongs to a cluster, and they belong to the same mode
            if (clusters[j]!=-1 && l2Distance(mean, &modes[clusters[j] * dim], dim) < bandwidth)
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

	//cout <<"Ho finito!!!!"<<endl;

	return clustersCount;
}


// todo: convert from RGB to XYZ to L*U*V*
// todo: convert from RGB to HSV (prof advice)

int main()
{
    // OPEN IMAGE PPM
    PPM ppmIn;
    int status;
    std::string inFilepath = "../Image/image_bigger.ppm";
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

    pixels modes;
    modes.R= new float[width*height];
    modes.G= new float[width*height];
    modes.B= new float[width*height];
    modes.X= new float[width*height];
    modes.Y= new float[width*height];

    int clusters[width*height];

    cout<<"Chiamo la funzione meanShiftTest"<<endl;
    auto start_time = high_resolution_clock::now();
    int numOfClusters= meanShift(Pixels,width*height,modes, clusters,0.4);
    auto end_time = high_resolution_clock::now();
    cout << "SoA duration " << duration_cast<microseconds>
                                       (end_time - start_time).count() / 1000.f << " ms" << endl;

    delete[] Pixels.R;
    delete[] Pixels.G;
    delete[] Pixels.B;
    delete[] Pixels.X;
    delete[] Pixels.Y;

    uint8_t buffer_image_new[width*height*3];

    for(int l=0; l<width*height; l++){
        cout<<clusters[l]<<", ";
    }
    cout<<endl;

    int k=0;
    for(int i=0; i<width*height*3; i+=3){
        buffer_image_new[i]=(uint8_t)(modes.R[clusters[k]]*255);
        buffer_image_new[i+1]=(uint8_t)(modes.G[clusters[k]]*255);
        buffer_image_new[i+2]=(uint8_t)(modes.B[clusters[k]]*255);
        k++;
    }

    delete[] modes.R;
    delete[] modes.G;
    delete[] modes.B;
    delete[] modes.X;
    delete[] modes.Y;

    ppmIn.load(buffer_image_new,height,width,ppmIn.getMax(),ppmIn.getMagic());
    std::string outFilepath = "../Image/image_bigger_out.ppm";
    status = ppmIn.write(outFilepath);
    cout <<status<<endl;
    cout <<"Nuova immagine salvata"<<endl;
    cout <<"Numero di Cluster: "<<numOfClusters<<endl;


    /*
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
    auto start_time = high_resolution_clock::now();
    int numOfClusters= meanShiftTest(points,width*height,dimension,clusters,modes,0.4);
    auto end_time = high_resolution_clock::now();
    cout << "SoA duration " << duration_cast<microseconds>
                                       (end_time - start_time).count() / 1000.f << " ms" << endl;

    uint8_t buffer_image_new[width*height*3];

    int k=0;
    for(int i=0; i<width*height*3; i+=3){
        buffer_image_new[i]=(uint8_t)(modes[clusters[k]*dimension]*255);
        buffer_image_new[i+1]=(uint8_t)(modes[clusters[k]*dimension+1]*255);
        buffer_image_new[i+2]=(uint8_t)(modes[clusters[k]*dimension+2]*255);
        k++;
    }

    ppmIn.load(buffer_image_new,height,width,ppmIn.getMax(),ppmIn.getMagic());
    std::string outFilepath = "../Image/image_bigger_out.ppm";
    status = ppmIn.write(outFilepath);
    cout <<status<<endl;
    cout <<"Nuova immagine salvata"<<endl;
    cout <<"Numero di Cluster: "<<numOfClusters<<endl;
    */
}
