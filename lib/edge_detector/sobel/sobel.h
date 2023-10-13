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
    double ** applySobel(double** image, int width, int height);
};

double ** Sobel::applySobel(double** image, int width, int height){
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
    double** gradient=u.initMatrix(width,height,0.0);
    for (size_t i = 0; i <height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            int gradientValue=int((sqrt(pow(2,matrixGx[i][j])+pow(2,matrixGy[i][j])))/255);
            gradient[i][j]=gradientValue>0?1:0;//>1?0:gradientValue;
        }
        
    }
    
    return gradient;
}
