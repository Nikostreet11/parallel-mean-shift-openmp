#include <iostream>
#include "libs/ppm_io.h"
#include <cmath>
using namespace std;

// SOA
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

// receive matrix of pixels in 5 multidimensional space R, G, B, X, Y
// TODO return vector of cluster indices
float* meanShift(pixels &points, size_t nOfPoints, pixels &Modes)
{
    for (size_t i = 0; i < nOfPoints; ++i)
    {
        for (size_t j = 0; j < nOfPoints; ++j){
            // todo if in distance...
        }
    }
    return NULL;
}

// todo convertion from RGB to XYZ to L*u*v*

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
