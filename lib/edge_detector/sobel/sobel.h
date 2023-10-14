#include <vector>
#include <iostream>
#include <thread>

/**
 * @author Ismael Farinango - 2023
 * @brief Algoritmo Sobel para le deteccion de bordes.
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
     * @brief Funcion que realiza la operacion de sobel, para la deteccion de bordes
     * @param image Matriz puntero de tipo double ** que contine la imagen a procesar.
     * @param width Numero de columnas de la imagen
     * @param height NUmero de filas de la imagen
     * @param thresh Umbral de binarizacion (opcional)
    */
    int ** applySobel(double** image, int width, int height, double thresh=0.0);
};

int ** Sobel::applySobel(double** image, int width, int height, double thresh){
    double** matrixGx=nullptr;
    double** matrixGy=nullptr;
    std::thread threadMGX([&](){
        matrixGx=f.conv2d(image,width,height,this->horizontalKernel);
    });

    std::thread threadMGY([&](){
        matrixGy=f.conv2d(image,width,height,this->verticalKernel);
    });

    threadMGX.join();
    threadMGY.join();
    int** gradient=u.initMatrix(width,height,0);
    for (size_t i = 0; i <height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            double gradientValue=int((sqrt(pow(matrixGx[i][j],2)+pow(matrixGy[i][j],2)))/255);
            gradient[i][j]=gradientValue>thresh?1:0;
        }
        
    }

    u.free_memory(matrixGx,height);
    u.free_memory(matrixGy,height);
    u.free_memory(image,height);
    
    return gradient;
}
