#pragma once

#include <vector>
#include <omp.h> // Asegúrate de incluir la librería de OpenMP

class FilterProcess
{
private:
    Utility u;

public:
    FilterProcess(/* args */);

    double convolution(double **&image, short int positionX, short int positionY, double **kernel, short int kernelSize);
    double convolution(double **&image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel);
    double **zeroPadding(double **&image, short int &width, short int &height, short int kernelSize);
    double **conv2d(double **image, short int width, short int height, std::vector<std::vector<int>> kernel);
    int **conv2d(double **image, short int width, short int height, std::vector<std::vector<double>> kernel);
};

FilterProcess::FilterProcess(/* args */)
{
}

double **FilterProcess::zeroPadding(double **&image, short int &width, short int &height, short int kernelSize)
{
    short int paddingSize = (kernelSize / 2) * 2;
    short int index = kernelSize / 2;
    auto **imagePadding = u.initMatrix((width + paddingSize), (height + paddingSize), 0.0);

#pragma omp parallel for collapse(2)
    for (short int i = index; i < (height + index); i++)
    {
        for (short int j = index; j < (width + index); j++)
        {
            imagePadding[i][j] = image[i - index][j - index];
        }
    }

    width = (width + paddingSize);
    height = (height + paddingSize);
    return imagePadding;
}

double FilterProcess::convolution(double **&image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel)
{
    double sum = 0.0;
    int kernelRow = 0, kernelCol = 0;
    for (int row = positionY; row < (kernel.size() + positionY); row++)
    {
        for (int col = positionX; col < (kernel.size() + positionX); col++)
        {
            sum += (image[row][col] * kernel[kernelRow][kernelCol]);
            kernelCol++;
        }
        kernelCol = 0;
        kernelRow++;
    }

    return sum;
}

double FilterProcess::convolution(double **&image, short int positionX, short int positionY, double **gaussianKernel, short int kernelSize)
{
    double sum = 0.0;
    int gaussianKernelRow = 0, gaussianKernelCol = 0;
    for (int row = positionY; row < (kernelSize + positionY); row++)
    {
        for (int col = positionX; col < (kernelSize + positionX); col++)
        {
            sum += (image[row][col] * gaussianKernel[gaussianKernelRow][gaussianKernelCol]);
            gaussianKernelCol++;
        }
        gaussianKernelCol = 0;
        gaussianKernelRow++;
    }

    return sum;
}

double **FilterProcess::conv2d(double **image, short int width, short int height, std::vector<std::vector<int>> kernel)
{
    double **result = u.initMatrix(width, height, 0.0);
    double **matrixPadding = zeroPadding(image, width, height, kernel.size());
    short int row = height - kernel.size();
    short int col = width - kernel.size();

#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernel);
        }
    }

    u.free_memory(matrixPadding, height);
    return result;
}

inline int **FilterProcess::conv2d(double **image, short int width, short int height, std::vector<std::vector<double>> kernel)
{
    int **result = u.initMatrix(width, height, 0);
    double **matrixPadding = zeroPadding(image, width, height, kernel.size());
    short int row = height - kernel.size();
    short int col = width - kernel.size();
    double **kernelCopy = u.initMatrix(kernel.size(), kernel.size(), 0.0);

#pragma omp parallel for collapse(2)
    for (int i = 0; i < kernel.size(); i++)
    {
        for (int j = 0; j < kernel.size(); j++)
        {
            kernelCopy[i][j] = kernel[i][j];
        }
    }

    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernelCopy, kernel.size());
        }
    }

    u.free_memory(matrixPadding, height);
    u.free_memory(kernelCopy, kernel.size());
    return result;
}
