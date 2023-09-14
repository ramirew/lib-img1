#include "./stb-image/stb_image.h"
extern "C"
{
#define STB_IMAGE_IMPLEMENTATION
#include "./stb-image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb-image/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./stb-image/stb_image_resize.h"
}
/**
 * @author Ing. Ismael Farinango - 2023
 * @details Contiene las funciones para leer y escribir imagenes, utiliza la libreria stb_image
*/
typedef double **stb_image;
class StbImageImplementation
{
private:
    int **img;
    int row;
    int col;
    const char *name;
    char *imgc;
    double ** stroke(double **image, int &width, int &height, int x, int y);
public:
    /**
     * @brief Lee imagenes en formato jpeg
     * @param absolutPath Ruta absoluta de la imagen a leer
     * @param width Variable por referencia para numero de columnas de la imagen
     * @param height Variable por referencia para numero de filas de la imagen
    */
    stb_image imread(const char *absolutPath, int &width, int &height);
    /**
     * @brief Escribe una imagen en formato jpg, jpeg
     * @param absolutPath Ruta donde se va a guardar la imagen
     * @param image Matriz que contiene la imagen
     * @param width Numero de columnas de la imagen
     * @param height Numero de fila de la imagen
    */
    void imshow(const char *absolutPath, stb_image image, int width, int height);
        /**
     * @brief Escribe una imagen en formato jpg, jpeg
     * @param absolutPath Ruta donde se va a guardar la imagen
     * @param image Matriz que contiene la imagen
     * @param width Numero de columnas de la imagen
     * @param height Numero de fila de la imagen
    */
    void imshow(const char *absolutPath, int ** image, int width, int height);
    /**
     * @brief Redimenciona la imagen
     * @param image Matriz que contiene la imagen
     * @param width columnas de la imagen
     * @param height Filas de la imagen
     * @param newWidth Columnas redimencionadas
     * @param newHeight Filas redimencionadas
    */
    double** imresize(double ** image, int width, int height, int newWidth, int newHeight);

    double ** imread(const char* absolutpath,int &width, int &height, double x, double y);

    StbImageImplementation(/* args */);
   // ~StbImageImplementation();
};

double ** StbImageImplementation::stroke(double **image, int &width, int &height, int x, int y){
    Utility u;
    stb_image stroke=u.initMatrix(width-(2*x),height-(2*y),0.0);

    for(int i=y;i<height-y;i++){
        for (size_t j = x; j < width-x; j++)
        {
              stroke[i-y][j-x]=image[i][j];
        }
        
    }
    u.free_memory(image,height);
    width=width-(2*x);
    height=height-(2*y);
    return stroke;
}

double** StbImageImplementation::imresize(double ** image, int width, int height, int newWidth, int newHeight){
    Utility u;
    float* img=new float[width*height];
    float *resize=new float[newWidth*newHeight];
    int cont=0;
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            img[cont]=float(image[i][j]);
            cont++;
        }
    }
     
    stbir_resize_float(img,width,height,0,resize,newWidth,newHeight,0,1);
    u.free_memory(image,height);
    delete[] img;
    return u.vect2mat(resize,(newHeight*newWidth),newWidth,newHeight);
}

stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height)
{
    /*
    * absolutPath: recibe la ruta absoluta de la imagen y el nobre de le imagen
    * Ejemplo: /home/user/img/imagen.jpg
    */
    GaussianFilter gaussian;
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    stb_image img = gaussian.initMatrix(width, height);
    int cont = 0;
    //printf("Imagen cargada, canales: %d\t, size %d x %d",n, width, height);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
           *(*(img + i) + j) = double(*(data + cont));
          /* int index=(i*width + j)*n;
           double r=(double)data[index];
           double g=(double)data[index +1];
           double b=(double)data[index +2];
           img[i][j]= (r+g+b)/3.0;//(double)data[cont];*/
            cont++;
        }
    }

    stbi_image_free(data);

    return img;
}

stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height, double x, double y)
{
    /*
    * absolutPath: recibe la ruta absoluta de la imagen y el nobre de le imagen
    * Ejemplo: /home/user/img/imagen.jpg
    */
    GaussianFilter gaussian;
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    stb_image img = gaussian.initMatrix(width, height);
    int cont = 0;
    //printf("Imagen cargada, canales: %d\t, size %d x %d",n, width, height);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
           *(*(img + i) + j) = double(*(data + cont));

            cont++;
        }
    }

    stbi_image_free(data);
    int sx=width*x;
    int sy=height*y;
    img=stroke(img,width,height,sx,sy);
    return img;
}

void StbImageImplementation::imshow(const char *absolutePath, stb_image image, int width, int height){
        char *img=new char[width*height];
        int index=0;
        for (short int i = 0; i < height; i++)
        {
            for (short int j = 0; j < width; j++)
            {
                img[index]=char(int(image[i][j])*255);
                index++;
            }
            
        }
        stbi_write_jpg(absolutePath,width,height,1,img,100);
        delete[] img;
        
}

void StbImageImplementation::imshow(const char *absolutePath, int ** image, int width, int height){
        char *img=new char[width*height];
        int index=0;
        for (short int i = 0; i < height; i++)
        {
            for (short int j = 0; j < width; j++)
            {
                img[index]=char(image[i][j]*255);
                index++;
            }
            
        }
        stbi_write_jpg(absolutePath,width,height,1,img,100);
        delete[] img;
        
}

StbImageImplementation::StbImageImplementation(/* args */)
{
}

