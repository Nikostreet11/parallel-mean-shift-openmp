#include <iostream>
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

int main()
{
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
}
