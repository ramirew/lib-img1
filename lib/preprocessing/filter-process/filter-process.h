#include <vector>
#include <chrono>
#include <iostream>
#include "/home/mickel/Documents/parcial2/proyectoGit/examen-paralela/resource_usage.h"
#include <omp.h>

/**
 * @author Ismael Farinango 
 * @brief Clase que contiene funciones necesarias para aplicar filtros como canny, gauss, sobel.
 * @version 1.0
 */
class FilterProcess
{
private:
    Utility u;
public:
    FilterProcess(/* args */);

    double convolution(double ** &image, short int positionX, short int positionY, double ** kernel, short int kernelSize);
    double convolution(double** &image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel);
    double** zeroPadding(double ** &image, short int &width, short int &height, short int kernelSize);
    double** conv2d(double** image, short int width, short int height, std::vector<std::vector<int>> kernel);
    int ** conv2d(double** image, short int width, short int height, std::vector<std::vector<double>> kernel);
};

FilterProcess::FilterProcess(/* args */)
{
}

double** FilterProcess::zeroPadding(double ** &image, short int &width, short int &height, short int kernelSize)
{
    short int paddingSize = (kernelSize / 2) * 2;
    short int index = kernelSize / 2;
    auto** imagePadding = u.initMatrix((width + paddingSize), (height + paddingSize), 0.0);

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    #pragma omp parallel for collapse(2)
    for (short int i = index; i < (height + index); i++)
    {
        for (short int j = index; j < (width + index); j++)
        {
            imagePadding[i][j] = image[i - index][j - index];
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION zeroPadding DEL ARCHIVO FilterProcess.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    width = (width + paddingSize);
    height = (height + paddingSize);
    return imagePadding;
}

double FilterProcess::convolution(double** &image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel)
{
    double sum = 0.0;
    int kernelRow = 0, kernelCol = 0;

    for (int row = positionY; row < (kernel.size() + positionY); row++)
    {
        for (int col = positionX; col < (kernel.size() + positionX); col++)
        {
            sum = sum + (image[row][col] * kernel[kernelRow][kernelCol]);
            kernelCol++;
        }
        kernelCol = 0;
        kernelRow++;
    }

    return sum;
}

double FilterProcess::convolution(double** &image, short int positionX, short int positionY, double** gaussianKernel, short int kernelSize)
{
    double sum = 0.0;
    int gaussianKernelRow = 0, gaussianKernelCol = 0;

    for (int row = positionY; row < (kernelSize + positionY); row++)
    {
        for (int col = positionX; col < (kernelSize + positionX); col++)
        {
            sum = sum + (image[row][col] * gaussianKernel[gaussianKernelRow][gaussianKernelCol]);
            gaussianKernelCol++;
        }
        gaussianKernelCol = 0;
        gaussianKernelRow++;
    }

    return sum;
}

double** FilterProcess::conv2d(double** image, short int width, short int height, std::vector<std::vector<int>> kernel)
{
    double **result = u.initMatrix(width, height, 0.0);
    double** matrixPadding = zeroPadding(image, width, height, kernel.size());
    short int row = height - kernel.size();
    short int col = width - kernel.size();

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    #pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernel);
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION conv2d (int kernel) DEL ARCHIVO FilterProcess.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    u.free_memory(matrixPadding, height);
    return result;
}

int** FilterProcess::conv2d(double **image, short int width, short int height, std::vector<std::vector<double>> kernel)
{
    int **result = u.initMatrix(width, height, 0);
    double** matrixPadding = zeroPadding(image, width, height, kernel.size());
    short int row = height - kernel.size();
    short int col = width - kernel.size();
    double **kernelCopy = u.initMatrix(kernel.size(), kernel.size(), 0.0);

    #pragma omp parallel for collapse(2)
    for(int i = 0; i < kernel.size(); i++)
    {
        for(int j = 0; j < kernel.size(); j++)
        {
            kernelCopy[i][j] = kernel[i][j];
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    #pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernelCopy, kernel.size());
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION conv2d (double kernel) DEL ARCHIVO FilterProcess.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    u.free_memory(matrixPadding, height);
    u.free_memory(kernelCopy, kernel.size());
    return result;
}
