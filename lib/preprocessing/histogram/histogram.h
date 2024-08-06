#include <vector>
#include <chrono>
#include <iostream>
#include "/home/mickel/Documents/parcial2/proyectoGit/examen-paralela/resource_usage.h"
#include <omp.h>

class HistogramHandler
{
private:
    Utility u;

public:
    std::vector<int> imhist(double **image, int width, int height);
    int **histeq(double **image, int width, int height);
};

std::vector<int> HistogramHandler::imhist(double **image, int width, int height)
{
    std::vector<int> hist(256, 0);

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    #pragma omp parallel for collapse(2) reduction(+:hist[:256])
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            hist[static_cast<int>(image[i][j])]++;
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION imhist DEL ARCHIVO HistogramHandler.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    return hist;
}

int **HistogramHandler::histeq(double **image, int width, int height)
{
    int **imageEq = u.initMatrix(width, height, 0);

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    std::vector<int> histogram = imhist(image, width, height);
    int pixels = width * height;
    int cdf[256] = {0}; // cdf calculo de la funcion de distribucion acumulada.
    double sumCDF = 0;

    #pragma omp parallel for reduction(+:sumCDF)
    for (int i = 0; i < 256; i++)
    {
        sumCDF = sumCDF + static_cast<double>(histogram[i]) / static_cast<double>(pixels);
        cdf[i] = sumCDF * 255.0;
    }

    #pragma omp parallel for collapse(2)
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            imageEq[i][j] = cdf[static_cast<int>(image[i][j])];
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION histeq DEL ARCHIVO HistogramHandler.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    u.free_memory(image, height);
    return imageEq;
}
