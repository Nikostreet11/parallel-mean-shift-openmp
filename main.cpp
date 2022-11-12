#include <iostream>
#include "libs/ppm_io.h"
using namespace std;



// receive matrix of points in any multidimensional space
// TODO return vector of cluster indices
int* meanShift(float** points, size_t nOfPoints, size_t dimension)
{
	// just print the array
	cout << __func__ << endl;
	for (size_t i = 0; i < nOfPoints; ++i)
	{
		cout << i << ":" << '\t';
		for (size_t j = 0; j < dimension; ++j)
			cout << points[i][j] << '\t';
		cout << endl;
	}
	return NULL;
}

struct pixels{
    int* R;
    int* G;
    int* B;
    int* X;
    int* Y;
};


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
    Pixels.R= new int[width*height];
    Pixels.G= new int[width*height];
    Pixels.B= new int[width*height];
    Pixels.X= new int[width*height];
    Pixels.Y= new int[width*height];

    int j=0;
    for(int i=0; i<width*height*3; i++){
        if(i%3==0){
            Pixels.R[j]=buffer_image[i];
        } else if (i%3==1){
            Pixels.G[j]=buffer_image[i];
        } else{
            Pixels.B[j]=buffer_image[i];
            Pixels.X[j]=(i/3)%width;
            Pixels.Y[j]=(i/3)/width;
            j++;
        }
    }

    cout <<"R: ";
    for(int i=0; i<width*height; i++){
        cout <<Pixels.R[i]<<" ";
    }
    cout <<endl;

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
