#include <vector>
class MediaFilter
{
private:
    FilterProcess f;
    int divFactor=10;
    std::vector<std::vector<double>> kernel={{1,1,1},{1,2,1},{1,1,1}};
public:
MediaFilter();
/**
 * @brief Filtro mediano
 * @param image Matris puntero de tipo double **, contiene la imagen a procesar
 * @param width Numero de columnas de la imagen
 * @param height NUmero de filas de la imagen
 * @note Retorna una matriz tipo int**
*/
int ** medianFilter(double** image, short int width, short int height);

};
MediaFilter::MediaFilter(){
    for (size_t i = 0; i < this->kernel.size(); i++)
    {
        for (size_t j = 0; j < kernel.size(); j++)
        {
            kernel[i][j]=kernel[i][j]/static_cast<double>(this->divFactor);
        }
        
    }
    
}
 int **MediaFilter::medianFilter(double **image, short int width, short int height){
    return  f.conv2d(image,width,height,this->kernel);
 }


