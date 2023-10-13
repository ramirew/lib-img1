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
    kernel initMatrix(short int width, short int height);
    kernel gaussianKernel(short int kernelSize, double sigma);
    kernel gaussianFilter(kernel image, short int width, short int height, kernel gaussianKernel, short int kernelSize);
    kernel gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma);
    kernel gaussianFilter(kernel image, short int width, short int height, double sigma);
    
    //~GaussianFilter();
private:
    kernel initKernel(short int kernelSize, double value = 0.0);
    void normalize(short int kernelSize, double sum, kernel &kernel);
    void computeKernel(short int kernelSize, kernel &kernel, double sigma);
    int computeKernelSize(double sigma);
    void zeroPadding(kernel &image, short int &width, short int &height, short int kernelSize);
    void freeMemory(kernel &matrix, short int numRows);
    double comvolution(kernel &image, short int positionX, short int positionY, kernel gaussianKernel, short int kernelSize);
};

// GaussianFilter::GaussianFilter(/* args */)

#endif