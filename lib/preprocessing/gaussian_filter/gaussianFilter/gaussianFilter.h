#ifndef GAUSSIAN_FILTER_H
#define GAUSSIAN_FILTER_H
#include <cmath>
#include <iostream>
#include <cstring>
typedef double **kernel;
using namespace std;

/*
 * Autor: Ing. Ismael Farinango
 */
class GaussianFilter
{

public:
    GaussianFilter(/* args */);
    /**
     * @brief Calcula el kernel gaussiano
     * @param kernelSize Longitud del kernel
     * @param sigma Valor del sigmma
     * @note Retorna una matriz de tipo double** con el kernel gaussiano.
    */
    kernel gaussianKernel(short int kernelSize, double sigma);
    /**
     * @brief Aplica la operacion del filtro gaussiano
     * @param image Matriz puntero de tipo double** que contiene la imagen a procesar
     * @param width Numero de columnas de la imagen
     * @param height Numero de filas de la imagen
     * @param gaussianKernel Matriz tipo double** que contiene el kernel gaussiano
     * @param kernelSize Longitud del kernel
     * @note Retorna una matriz de tipo double** con la imagen procesada.
    */
    kernel gaussianFilter(kernel image, short int width, short int height, kernel gaussianKernel, short int kernelSize);
    /**
     * @brief Aplica la operacion del filtro gaussiano
     * @param image Matriz puntero de tipo double** que contiene la imagen a procesar
     * @param width Numero de columnas de la imagen
     * @param height Numero de filas de la imagen
     * @param kernelSize Longitud del kernel
     * @param sigma Valor del Sigmma
     * @note Retorna una matriz de tipo double** con la imagen procesada
    */   
    kernel gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma);
    /**
     * @brief Aplica la operacion del filtro gaussiano
     * @param image Matriz puntero de tipo double** que contiene la imagen a procesar
     * @param width Numero de columnas de la imagen
     * @param height Numero de filas de la imagen
     * @param sigma Valor del Sigmma
     * @note Retorna una matriz de tipo double** con la imagen procesada
    */     
    kernel gaussianFilter(kernel image, short int width, short int height, double sigma);
    
    //~GaussianFilter();
private:
    Utility u;
    FilterProcess f;
    /**
     * @brief Funcion encargada de inicializar la matriz del kernel
     * @param kernelSize Longitud del kernel
     * @param value Valor de inicializacion, por defecto 0
     * @note Retorna la matriz iniiclaizada con el valor asignado, tipo double **
    */
    kernel initKernel(short int kernelSize, double value = 0.0);
    /**
     * @brief Funcion que se encarga de normalizar la matriz del kernel
     * @param kernelSize LOngitud del kernel a normalizar
     * @param sum Valor acumulado del computo del kernel
     * @param kernel Matriz de tipo double** que contiene los valores del kernel
     * @note Funcion void
    */
    void normalize(short int kernelSize, double sum, kernel &kernel);
    /**
     * @brief Funcion encargada de calcular el kernel.
     * @param kernelSize Longitud del kernel
     * @param kernel Matriz inicializada de tipo double**
     * @param sigma Valor del Sigma
     * @note Funcion void
    */
    void computeKernel(short int kernelSize, kernel &kernel, double sigma);
    /**
     * @brief Funcion encargada de calcular la longitud del kernel en base al valor del sigma
     * @param sigma Valor del Sigma
     * @note Retorna un valor entero
    */
    int computeKernelSize(double sigma);
};

// GaussianFilter::GaussianFilter(/* args */)

#endif