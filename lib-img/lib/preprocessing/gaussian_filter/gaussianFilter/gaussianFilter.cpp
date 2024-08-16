#include "gaussianFilter.h"
#include <omp.h>
#include <cmath>
#include <vector>

/*
 * Autor: Ing, Ismael Farinango - 2023
 */
GaussianFilter::GaussianFilter()
{
    /* Constructor de la clase */
}

kernel GaussianFilter::initKernel(short int kernelSize, double value)
{
    /*
    ** En esta funcion se inicializa la matriz del kernel.
    */
    kernel gausskernel = new double *[kernelSize];
    for (int i = 0; i < kernelSize; ++i)
    {
        gausskernel[i] = new double[kernelSize];
        for (short int j = 0; j < kernelSize; j++)
        {
            gausskernel[i][j] = value;
        }
    }

    return gausskernel;
}

void GaussianFilter::normalize(short int kernelSize, double sum, kernel &gaussian)
{
/*
 * Esta funcion es la encargada de normalizar el Kernel, que la suma no supere 1.
 */
#pragma omp parallel for collapse(2)
    for (short int i = 0; i < kernelSize; ++i)
    {
        for (short int j = 0; j < kernelSize; ++j)
        {
            gaussian[i][j] /= sum;
        }
    }
}

void GaussianFilter::computeKernel(short int kernelSize, kernel &kernel, double sigma)
{
    /*
     * Se calcula los valores del kernel
     */
    int center = kernelSize / 2;
    double sum = 0.0;
#pragma omp parallel for collapse(2) reduction(+ : sum)
    for (short int x = 0; x < kernelSize; ++x)
    {
        for (short int y = 0; y < kernelSize; ++y)
        {
            kernel[x][y] = (1 / sqrt((2 * M_PI * (sigma * sigma)))) * pow(M_E, -((pow((x - center), 2) + pow((y - center), 2)) / (2 * sigma * sigma)));
            sum += kernel[x][y];
        }
    }
    normalize(kernelSize, sum, kernel);
}

kernel GaussianFilter::gaussianKernel(short int kernelSize, double sigma)
{
    /*
     * Funcion principal encargada de generar el kernel gausiano.
     */
    kernel gaussianKernel = initKernel(kernelSize);
    computeKernel(kernelSize, gaussianKernel, sigma);
    return gaussianKernel;
}

int GaussianFilter::computeKernelSize(double sigma)
{
    /*
     * Calcula la longitud del kernel
     */
    int scaleFactor = 3;
    int kernelSize = round(scaleFactor * sigma);
    kernelSize = (kernelSize % 2 == 0) ? kernelSize + 1 : kernelSize;
    return kernelSize;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, kernel gaussianKernel, short int kernelSize)
{
    int auxHeight = height;
    // Inicializa la matriz donde se va a almacenar los resultados
    kernel result = this->u.initMatrix(width, height, 0.0);
    // Los valores de width y height después del zeropadding cambian.
    double **zeropadding = this->f.zeroPadding(image, width, height, kernelSize);
    // Se calcula los nuevos números de filas y columnas
    short int row = height - kernelSize;
    short int col = width - kernelSize;

#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // Realiza la operación de la convolución de cada pixel de la imagen con el kernel
            result[y][x] = this->f.convolution(zeropadding, x, y, gaussianKernel, kernelSize);
        }
    }
    // Libera la memoria de la matriz que contiene la imagen
    this->u.free_memory(image, auxHeight);
    this->u.free_memory(zeropadding, height);
    // Retorna la matriz resultante.
    return result;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma)
{
    int auxHeight = height;
    // Inicializa la matriz donde se va a almacenar los resultados
    kernel result = this->u.initMatrix(width, height, 0.0);
    // Se crea el kernel gaussiano
    kernel __gaussianKernel = gaussianKernel(kernelSize, sigma);
    // Los valores de width y height después del zeropadding cambian.
    double **zeropadding = this->f.zeroPadding(image, width, height, kernelSize);
    // Se calcula los nuevos números de filas y columnas
    short int row = height - kernelSize;
    short int col = width - kernelSize;

#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // Realiza la operación de la convolución de cada pixel de la imagen con el kernel
            result[y][x] = this->f.convolution(zeropadding, x, y, __gaussianKernel, kernelSize);
        }
    }
    // Libera la memoria de la matriz que contiene el kernel
    this->u.free_memory(__gaussianKernel, kernelSize);
    // Libera la memoria de la matriz que contiene la imagen
    this->u.free_memory(image, auxHeight);
    this->u.free_memory(zeropadding, height);
    // Retorna la matriz resultante.
    return result;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, double sigma)
{
    return gaussianFilter(image, width, height, computeKernelSize(sigma), sigma);
}