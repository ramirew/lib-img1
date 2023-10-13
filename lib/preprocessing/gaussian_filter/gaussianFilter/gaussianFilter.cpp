#include "gaussianFilter.h"

/*
 * Autor: Ing, Ismael Farinango - 2023
 */
GaussianFilter::GaussianFilter()
{
    /*Constructor de la clase*/
}
kernel GaussianFilter::initMatrix(short int width, short int height)
{
    kernel matrix = new double *[height];
    for (size_t i = 0; i < height; i++)
    {
        matrix[i] = new double[width];
        for (short int j = 0; j < width; j++)
        {
            matrix[i][j] = 0.0;
        }
    }
    // memset(matrix[0], 0.0, width * height * sizeof(double));
    return matrix;
}
void GaussianFilter::freeMemory(kernel &matrix, short int numRows)
{
    // Libera la memoria de la matriz  n x n
    for (short int i = 0; i < numRows; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
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

void GaussianFilter::zeroPadding(kernel &image, short int &width, short int &height, short int kernelSize)
{
    // Calcula el padding  que se le va a agregar
    short int paddingSize = (kernelSize / 2) * 2;
    // Clacular el index, este se encargar de ubicar el pixel de la imagen en la matriz padding
    short int index = kernelSize / 2;
    // Se crea una nueva matriz que contedra la imagen aumentado el padding.
    kernel imagePadding = initMatrix((width + paddingSize), (height + paddingSize));
    // Agregar el paddig a la matriz de la imagen
    // double aux=0;
    for (short int i = index; i < (height + index); i++)
    {
        for (short int j = index; j < (width + index); j++)
        {
            // aux=image[i-index][j-index];
            imagePadding[i][j] = image[i - index][j - index];
        }
    }
    int j = 0;
    freeMemory(image, height);
    width = (width + paddingSize);
    height = (height + paddingSize);
    image = imagePadding;
}

double GaussianFilter::comvolution(kernel &image, short int positionX, short int positionY, kernel gaussianKernel, short int kernelSize)
{
    /*
     * positionX y positionY corresponden a la posicion donde se encuentra el pixel de la imagen que va a ser comvulusionado.
     */
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

int GaussianFilter::computeKernelSize(double sigma){
    /*
    * Calcula la longitud del kernel
    */
    int scaleFactor=3;
    int kernelSize=round(scaleFactor*sigma);
    kernelSize=(kernelSize % 2 ==0)?kernelSize+1:kernelSize;
    return kernelSize;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, kernel gaussianKernel, short int kernelSize)
{
    return 0;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, short int kernelSize, double sigma)
{
  
    // Inicializa la matriz donde se va ha almacenar los resutados
    kernel result = initMatrix(width, height);
    // Se crea el kernel gaussiano
    kernel __gaussianKernel = gaussianKernel(kernelSize, sigma);
    // los valores de width, y height despues del zeropadding cambian.
    zeroPadding(image, width, height, kernelSize);
    // se calcula los nuevos numeros de filas y columnas
   
    short int row = height - kernelSize;
    short int col = width - kernelSize;

    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // realiza la operacion de la comvolucion de cada pixel de la imagen con el kernel
            result[y][x] = comvolution(image, x, y, __gaussianKernel, kernelSize);
           // printf("%f\t",result[y][x]);
        }
       // printf("\n");
    }
    // libera la memoria de la matriz que contiene el kernel
    freeMemory(__gaussianKernel, kernelSize);
    // Libera la memoria de la matriz que contiene la imagen
    freeMemory(image, height);
    //retorna la matriz resultante.
    return result;
}

kernel GaussianFilter::gaussianFilter(kernel image, short int width, short int height, double sigma){
    
    return gaussianFilter(image,width,height,computeKernelSize(sigma),sigma);
}