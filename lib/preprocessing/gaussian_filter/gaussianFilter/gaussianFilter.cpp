#include "gaussianFilter.h"
#include <chrono>
#include "/home/mickel/Documents/parcial2/proyectoGit/examen-paralela/resource_usage.h"
#include <omp.h>

/*
 * Autor: Ing, Ismael Farinango - 2023
 */
GaussianFilter::GaussianFilter()
{
    /*Constructor de la clase*/
}

kernel GaussianFilter::initKernel(short int kernelSize, double value)
{
    /*
    ** En esta funcion se inicializa la matriz del kernel.
    */
    auto start = std::chrono::high_resolution_clock::now();

    kernel gausskernel = new double *[kernelSize];
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    //#pragma omp parallel for
    for (int i = 0; i < kernelSize; ++i)
    {
        gausskernel[i] = new double[kernelSize];
        for (short int j = 0; j < kernelSize; j++)
        {
            gausskernel[i][j] = value;
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION initKernel DEL ARCHIVO gaussianFilter.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    return gausskernel;
}

void GaussianFilter::normalize(short int kernelSize, double sum, kernel &gaussian)
{
    /*
     * Esta funcion es la encargada de normalizar el Kernel, que la suma no supere 1.
     */
    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    //#pragma omp parallel for collapse(2)
    for (short int i = 0; i < kernelSize; ++i)
    {
        for (short int j = 0; j < kernelSize; ++j)
        {
            gaussian[i][j] /= sum;
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION normalize DEL ARCHIVO gaussianFilter.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;
}

void GaussianFilter::computeKernel(short int kernelSize, kernel &kernel, double sigma)
{
    /*
     * Se calcula los valores del kernel
     */
    int center = kernelSize / 2;
    double sum = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    //#pragma omp parallel for collapse(2) reduction(+:sum)
    for (short int x = 0; x < kernelSize; ++x)
    {
        for (short int y = 0; y < kernelSize; ++y)
        {
            kernel[x][y] = (1 / sqrt((2 * M_PI * (sigma * sigma)))) * pow(M_E, -((pow((x - center), 2) + pow((y - center), 2)) / (2 * sigma * sigma)));
            sum += kernel[x][y];
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION computeKernel DEL ARCHIVO gaussianFilter.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

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

    // Inicializa la matriz donde se va ha almacenar los resultados
    kernel result = this->u.initMatrix(width, height, 0.0);

    // los valores de width, y height después del zeropadding cambian.
    double **zeropadding = this->f.zeroPadding(image, width, height, kernelSize);

    // se calcula los nuevos números de filas y columnas
    short int row = height - kernelSize;
    short int col = width - kernelSize;

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    //#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // realiza la operación de la convolución de cada pixel de la imagen con el kernel
            result[y][x] = this->f.convolution(zeropadding, x, y, gaussianKernel, kernelSize);
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION gaussianFilter (con kernel) DEL ARCHIVO gaussianFilter.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    // Libera la memoria de la matriz que contiene la imagen
    this->u.free_memory(image, auxHeight);
    this->u.free_memory(zeropadding, height);

    // retorna la matriz resultante.
    return result;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma)
{
    int auxHeight = height;

    // Inicializa la matriz donde se va ha almacenar los resultados
    kernel result = this->u.initMatrix(width, height, 0.0);

    // Se crea el kernel gaussiano
    kernel __gaussianKernel = gaussianKernel(kernelSize, sigma);

    // los valores de width, y height después del zeropadding cambian.
    double **zeropadding = this->f.zeroPadding(image, width, height, kernelSize);

    // se calcula los nuevos números de filas y columnas
    short int row = height - kernelSize;
    short int col = width - kernelSize;

    auto start = std::chrono::high_resolution_clock::now();
    double cpu_usage_before = getCpuUsage();
    long memory_usage_before = getMemoryUsage();

    //#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // realiza la operación de la convolución de cada pixel de la imagen con el kernel
            result[y][x] = this->f.convolution(zeropadding, x, y, __gaussianKernel, kernelSize);
        }
    }

    double cpu_usage_after = getCpuUsage();
    long memory_usage_after = getMemoryUsage();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "*******************************************************************"<< std:: endl;
    std::cout << "FUNCION gaussianFilter (con sigma) DEL ARCHIVO gaussianFilter.cpp: " << std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria antes: " << memory_usage_before << " KB" << std::endl;
    std::cout << "Uso de memoria después: " << memory_usage_after << " KB" << std::endl;
    std::cout << "Uso de memoria incrementado: " << (memory_usage_after - memory_usage_before) << " KB" << std::endl;
    std::cout << "Uso de CPU incrementado: " << (cpu_usage_after - cpu_usage_before) << " %\n";
    std::cout << "*******************************************************************"<< std::endl;

    // libera la memoria de la matriz que contiene el kernel
    this->u.free_memory(__gaussianKernel, kernelSize);

    // Libera la memoria de la matriz que contiene la imagen
    this->u.free_memory(image, auxHeight);
    this->u.free_memory(zeropadding, height);

    // retorna la matriz resultante.
    return result;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, double sigma)
{
    return gaussianFilter(image, width, height, computeKernelSize(sigma), sigma);
}
