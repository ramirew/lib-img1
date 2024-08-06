#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include "/home/mickel/Documents/parcial2/proyectoGit/examen-paralela/resource_usage.h"
#include <cmath>
#include <fstream>
#include <sstream>

/**
 * @author Ismael Farinango - 2023
 * @brief Algoritmo Sobel para la detección de bordes.
*/

class Sobel
{
private:
    std::vector<std::vector<int>> horizontalKernel={{-1,0,1},{-2,0,2},{-1,0,1}};
    std::vector<std::vector<int>> verticalKernel={{-1,-2,-1},{0,0,0},{1,2,1}};
    FilterProcess f;
    Utility u;
public:
    /**
     * @brief Función que realiza la operación de sobel, para la detección de bordes
     * @param image Matriz puntero de tipo double ** que contiene la imagen a procesar.
     * @param width Número de columnas de la imagen
     * @param height Número de filas de la imagen
     * @param thresh Umbral de binarización (opcional)
    */
    int ** applySobel(double** image, int width, int height, double thresh=0.0);
};

int ** Sobel::applySobel(double** image, int width, int height, double thresh)
{
    long initialMemory = getMemoryUsage();
    auto start = std::chrono::high_resolution_clock::now();

    double** matrixGx = nullptr;
    double** matrixGy = nullptr;

    std::thread threadMGX([&]() {
        matrixGx = f.conv2d(image, width, height, this->horizontalKernel);
    });

    std::thread threadMGY([&]() {
        matrixGy = f.conv2d(image, width, height, this->verticalKernel);
    });

    threadMGX.join();
    threadMGY.join();
    int** gradient = u.initMatrix(width, height, 0);

    #pragma omp parallel for
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            double gradientValue = int((sqrt(pow(matrixGx[i][j], 2) + pow(matrixGy[i][j], 2))) / 255);
            gradient[i][j] = gradientValue > thresh ? 1 : 0;
        }
    }

    u.free_memory(matrixGx, height);
    u.free_memory(matrixGy, height);
    u.free_memory(image, height);

    auto end = std::chrono::high_resolution_clock::now();
    long finalMemory = getMemoryUsage();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "*******************************************************************"<< std::endl;
    std::cout << "FUNCION applySobel DEL ARCHIVO Sobel.h: " <<std::endl;
    std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos" << std::endl;
    std::cout << "Uso de memoria: " << finalMemory - initialMemory << " KB" << std::endl;
    std::cout << "*******************************************************************"<< std::endl;
    
    return gradient;
}
