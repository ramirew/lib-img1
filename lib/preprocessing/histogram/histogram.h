#include <vector>
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
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            hist[image[i][j]]++;
        }
    }
    return hist;
}

int **HistogramHandler::histeq(double **image, int width, int height)
{
    int **imageEq = u.initMatrix(width, height, 0);
    std::vector<int> histogram = imhist(image, width, height);
    int pixels = width * height;
    int cdf[256] = {0}; // cdf calculo de la funcion de distribucion acumulada.
    double sumCDF=0;

    printf("%f",static_cast<double> (histogram[0])/static_cast<double>(pixels));
    for(int i=0;i<256;i++){
        sumCDF=sumCDF+static_cast<double> (histogram[i])/static_cast<double>(pixels);
        cdf[i]=sumCDF*255.0;
    }

    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            imageEq[i][j]=cdf[static_cast<int>(image[i][j])];
        }
        
    }
    
    
 
    u.free_memory(image, height);
    return imageEq;
}
