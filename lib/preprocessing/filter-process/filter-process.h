#include <vector>
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
 double convolution(double** &image,short int positionX, short int positionY, std::vector<std::vector<int>> kernel);
 /**
  * @brief Funcion encargada de ejecutar la operacion de Zero Padding
  * @param image Matriz puntero de tipo double, contiene la iamgen a procesar.
  * @param width Numero de columnas de la imagen
  * @param height Numero de filas de la imagen
  * @param kernelSize Longitud del kernel
 */
 double** zeroPadding(double ** &image, short int &width, short int &height, short int kernelSize);
 double** conv2d(double** image, short int width, short int height, std::vector<std::vector<int>> kernel);

     
};
 FilterProcess:: FilterProcess(/* args */)
{
}
double** FilterProcess::zeroPadding(double ** &image, short int &width, short int &height, short int kernelSize)
{
    // Calcula el padding  que se le va a agregar
    short int paddingSize = (kernelSize / 2) * 2;
    // Clacular el index, este se encargar de ubicar el pixel de la imagen en la matriz padding
    short int index = kernelSize / 2;
    // Se crea una nueva matriz que contedra la imagen aumentado el padding.
    auto** imagePadding = u.initMatrix((width + paddingSize), (height + paddingSize),0.0);
    // Agregar el paddig a la matriz de la imagen
    // double aux=0;
    for (short int i = index; i < (height + index); i++)
    {
        for (short int j = index; j < (width + index); j++)
        {
            imagePadding[i][j] = image[i - index][j - index];
        }
    }
    int j = 0;
    //u.free_memory(image, height);
    width = (width + paddingSize);
    height = (height + paddingSize);
    return imagePadding;
}

double FilterProcess::convolution(double** &image,short int positionX, short int positionY, std::vector<std::vector<int>> kernel){
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

 double** FilterProcess::conv2d(double** image, short int width, short int height, std::vector<std::vector<int>> kernel){
    double ** result=u.initMatrix(width,height,0.0);
    double** matrixPadding= zeroPadding(image,width,height,kernel.size());
     short int row = height - kernel.size();
    short int col = width - kernel.size();

    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            // realiza la operacion de la comvolucion de cada pixel de la imagen con el kernel
            result[y][x] = convolution(matrixPadding, x, y,kernel);
           // printf("%f\t",result[y][x]);
        }
       // printf("\n");
    }
    u.free_memory(matrixPadding,height);
    return result;
 }
