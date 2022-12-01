#include "include/ppm_io.h"
#include "matrix_meanshift.cpp"
#include "soa_meanshift.cpp"
#include "rgb_pixels.cpp"
#include <iostream>
#include <chrono>

#define DIMENSION 5
#define inputFilepath "../img/image.ppm"
#define outputFilepath "../img/image_out.ppm"

using namespace std;
using namespace chrono;

// todo: convert from RGB to XYZ to L*U*V*
// todo: convert from RGB to HSV (prof advice)
// todo: parallelize using OpenMP
// todo: parallelize using Cuda

int main()
{
	// open the ppm image
	PPM ppm;
	if (ppm.read(inputFilepath) != 0)
	{
		cout << "ERROR: failed to open the image";
		return -1;
	}

	int width = ppm.getW();
	int height = ppm.getH();
	uint8_t* buffer_image = ppm.getImageHandler();
/*
    // create the structure of arrays (soa)
    RgbPixels rgbPixels;
	rgbPixels.create(width, height);
	rgbPixels.load(buffer_image);

	RgbPixels modes;
	modes.create(width, height);

    int clusters[width * height];

    cout << "calling soaMeanShift..." << endl;

    auto start_time = high_resolution_clock::now();
    int numOfClusters = soaMeanShift(rgbPixels, width * height, modes, clusters, 0.4);
    auto end_time = high_resolution_clock::now();

    cout << "SoA duration " << duration_cast<microseconds> (end_time - start_time).count() / 1000.f << " ms" << endl;

    uint8_t buffer_image_new[width * height * 3];

    int k=0;
    for(int i=0; i<width*height*3; i+=3){
        buffer_image_new[i]=(uint8_t)(modes.r[clusters[k]]*255);
        buffer_image_new[i+1]=(uint8_t)(modes.g[clusters[k]]*255);
        buffer_image_new[i+2]=(uint8_t)(modes.b[clusters[k]]*255);
        k++;
    }

	rgbPixels.destroy();
    modes.destroy();
*/

	// points matrix (n x DIMENSION)
    float points[width * height * DIMENSION];

    int j = 0;
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

    cout<<"Chiamo la funzione matrixMeanShift"<<endl;
    auto start_time = high_resolution_clock::now();
    int numOfClusters = matrixMeanShift(points,width*height,dimension,clusters,modes,0.4);
    auto end_time = high_resolution_clock::now();
    cout << "duration " << duration_cast<microseconds>
                                       (end_time - start_time).count() / 1000.f << " ms" << endl;

    uint8_t buffer_image_new[width*height*3];

    int k=0;
    for(int i=0; i<width*height*3; i+=3) {
		buffer_image_new[i] = (uint8_t) (modes[clusters[k] * dimension] * 255);
		buffer_image_new[i + 1] = (uint8_t) (modes[clusters[k] * dimension + 1] * 255);
		buffer_image_new[i + 2] = (uint8_t) (modes[clusters[k] * dimension + 2] * 255);
		k++;
	}

    ppm.load(buffer_image_new, height, width, ppm.getMax(), ppm.getMagic());

	// write the output ppm image
	if (ppm.write(outputFilepath) != 0)
	{
		cout << "ERROR: failed to write the image";
		return -1;
	}

    cout << "new image saved" << endl;
    cout << "number of clusters: " << numOfClusters << endl;

	return 0;
}
