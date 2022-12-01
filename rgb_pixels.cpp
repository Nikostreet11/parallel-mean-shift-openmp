#ifndef RGB_PIXELS_CPP
#define RGB_PIXELS_CPP

// structure of arrays
struct RgbPixels {
	int width;
	int height;
	float* r;
	float* g;
	float* b;
	float* x;
	float* y;

	void create(int _width, int _height) {
		width = _width;
		height = _height;
		r = new float[width * height];
		g = new float[width * height];
		b = new float[width * height];
		x = new float[width * height];
		y = new float[width * height];
	}

	void destroy() {
		delete[] r;
		delete[] g;
		delete[] b;
		delete[] x;
		delete[] y;
	}

	void load(uint8_t* buffer_image) {
		int j = 0;
		for (int i = 0; i < width * height * 3; i++) {
			if (i % 3 == 0) {
				r[j] = (float) buffer_image[i] / 255;
			} else if (i % 3 == 1) {
				g[j] = (float) buffer_image[i] / 255;
			} else {
				b[j] = (float) buffer_image[i] / 255;
				x[j] = (float) ((i / 3) % width) / (width - 1);
				y[j] = (float) ((i / 3) / width) / (height - 1);
				j++;
			}
		}
	}

	void write(int i, float* array) {
		array[0] = r[i];
		array[1] = g[i];
		array[2] = b[i];
		array[3] = x[i];
		array[4] = y[i];
	}

	void read(int i, float* array) {

	}

	void print(int i) {
		cout << "[ " << r[i] << " " << g[i] << " " << b[i] << " " << x[i] << " " << y[i] << " ]";
	}
};

#endif // RGB_PIXELS_CPP