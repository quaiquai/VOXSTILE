
#include "noise.h"


// Function to compute the dot product of two std::vectors
float dot(const std::vector<float>& v1, const std::vector<float>& v2) {
	return v1[0] * v2[0] + v1[1] * v2[1];
}

// Shuffle function to shuffle a vector
void Shuffle(std::vector<int>& arrayToShuffle) {
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(arrayToShuffle.begin(), arrayToShuffle.end(), g);
}

// Function to create a permutation table
std::vector<int> MakePermutation() {
	std::vector<int> permutation(256);
	for (int i = 0; i < 256; i++) {
		permutation[i] = i;
	}

	Shuffle(permutation);

	// Duplicate the permutation array
	permutation.insert(permutation.end(), permutation.begin(), permutation.end());

	return permutation;
}

// Function to get a constant vector based on value from permutation table
std::vector<float> GetConstantVector(int v) {
	int h = v & 3;
	if (h == 0)
		return { 1.0f, 1.0f };
	else if (h == 1)
		return { -1.0f, 1.0f };
	else if (h == 2)
		return { -1.0f, -1.0f };
	else
		return { 1.0f, -1.0f };
}

// Fade function for smoothing
float Fade(float t) {
	return ((6 * t - 15) * t + 10) * t * t * t;
}

// Linear interpolation function
float Lerp(float t, float a1, float a2) {
	return a1 + t * (a2 - a1);
}

// Global permutation table
std::vector<int> Permutation = MakePermutation();

// Function to compute 2D Perlin Noise using std::vector<float> for points
float Noise2D(float x, float y) {
	int X = (int)std::floor(x) & 255;
	int Y = (int)std::floor(y) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	std::vector<float> topRight = { xf - 1.0f, yf - 1.0f };
	std::vector<float> topLeft = { xf, yf - 1.0f };
	std::vector<float> bottomRight = { xf - 1.0f, yf };
	std::vector<float> bottomLeft = { xf, yf };

	// Select a value from the permutation array for each of the 4 corners
	int valueTopRight = Permutation[Permutation[X + 1] + Y + 1];
	int valueTopLeft = Permutation[Permutation[X] + Y + 1];
	int valueBottomRight = Permutation[Permutation[X + 1] + Y];
	int valueBottomLeft = Permutation[Permutation[X] + Y];

	// Dot product for each corner
	float dotTopRight = dot(topRight, GetConstantVector(valueTopRight));
	float dotTopLeft = dot(topLeft, GetConstantVector(valueTopLeft));
	float dotBottomRight = dot(bottomRight, GetConstantVector(valueBottomRight));
	float dotBottomLeft = dot(bottomLeft, GetConstantVector(valueBottomLeft));

	// Fade values
	float u = Fade(xf);
	float v = Fade(yf);

	// Interpolate results
	return Lerp(u,
		Lerp(v, dotBottomLeft, dotTopLeft),
		Lerp(v, dotBottomRight, dotTopRight));
}