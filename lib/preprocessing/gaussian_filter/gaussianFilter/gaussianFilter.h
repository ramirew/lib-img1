#ifndef GAUSSIAN_FILTER_H
#define GAUSSIAN_FILTER_H
#include <cmath>
#include <iostream>
#include <cstring>
typedef double **kernel;
using namespace std;
//#include "../functions/StbImageImplementation.h"
/*
 * Autor: Ing. Ismael Farinango
 */
class GaussianFilter
{

public:
    GaussianFilter(/* args */);
    kernel gaussianKernel(short int kernelSize, double sigma);
    kernel gaussianFilter(kernel image, short int width, short int height, kernel gaussianKernel, short int kernelSize);
    kernel gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma);
    kernel gaussianFilter(kernel image, short int width, short int height, double sigma);
    
    //~GaussianFilter();
private:
    Utility u;
    FilterProcess f;
    kernel initKernel(short int kernelSize, double value = 0.0);
    void normalize(short int kernelSize, double sum, kernel &kernel);
    void computeKernel(short int kernelSize, kernel &kernel, double sigma);
    int computeKernelSize(double sigma);
};

// GaussianFilter::GaussianFilter(/* args */)

#endif