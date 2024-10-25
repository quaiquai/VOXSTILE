#ifndef NOISE_H
#define NOISE_H

#include <iostream>
#include <cmath>
#include <algorithm> // for std::shuffle
#include <random> // for random device and random engine
#include <vector>
#include <math.h>

// Function to compute the dot product of two std::vectors
float dot(const std::vector<float>& v1, const std::vector<float>& v2);

// Shuffle function to shuffle a vector
void Shuffle(std::vector<int>& arrayToShuffle);

// Function to create a permutation table
std::vector<int> MakePermutation();

// Function to get a constant vector based on value from permutation table
std::vector<float> GetConstantVector(int v);

// Fade function for smoothing
float Fade(float t);

// Linear interpolation function
float Lerp(float t, float a1, float a2);

// Function to compute 2D Perlin Noise using std::vector<float> for points
float Noise2D(float x, float y);

// Global permutation table (to be defined in the implementation file)
extern std::vector<int> Permutation;

#endif
