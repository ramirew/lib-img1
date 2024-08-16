#include "./stb-image/stb_image.h"
#include <omp.h>
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
 * @details Contiene las funciones para leer y escribir imagenes de tipo JPG y JPEG, utiliza la libreria stb_image
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
    double **stroke(double **image, int &width, int &height, int x, int y);
    Utility u;

public:
    /**
     * @brief Lee imagenes en formato jpeg, jpg
     * @param absolutPath Ruta absoluta de la imagen a leer
     * @param width Variable por referencia para numero de columnas de la imagen
     * @param height Variable por referencia para numero de filas de la imagen
     * @note La funcion solo soporta los formatos JPG, JPEG. Retorna matriz tipo double **
     */
    stb_image imread(const char *absolutPath, int &width, int &height);
    /**
     * @brief Escribe una imagen en formato jpg, jpeg
     * @param absolutPath Ruta donde se va a guardar la imagen
     * @param image Matriz que contiene la imagen
     * @param width Numero de columnas de la imagen
     * @param height Numero de fila de la imagen
     * @note La funcion soporta los formatos JPG, JPEG
     */
    void imwrite(const char *absolutPath, stb_image image, int width, int height);
    /**
     * @brief Escribe una imagen en formato jpg, jpeg
     * @param absolutPath Ruta donde se va a guardar la imagen
     * @param image Matriz que contiene la imagen
     * @param width Numero de columnas de la imagen
     * @param height Numero de fila de la imagen
     * @note La funcion soporta los formatos JPG, JPEG
     */
    void imwrite(const char *absolutPath, int **image, int width, int height);
    void imwrite(const char *absolutPath, unsigned char **image, int width, int height);
    /**
     * @brief Redimenciona la imagen
     * @param image Matriz que contiene la imagen
     * @param width columnas de la imagen
     * @param height Filas de la imagen
     * @param newWidth Columnas redimencionadas
     * @param newHeight Filas redimencionadas
     * @note La funcion retorna una matriz de tipo double ** con la imagen redimencionada
     */
    double **imresize(double **image, int width, int height, int newWidth, int newHeight);

    /**
     * @brief Lee imagenes en formato jpeg, jpg
     * @param absolutPath Ruta absoluta de la imagen a leer
     * @param width Variable por referencia para numero de columnas de la imagen
     * @param height Variable por referencia para numero de filas de la imagen
     * @param x Valo porcentual a desplzarse en x (ancho)
     * @param y Valor porcentual a desplazarse en y (alto)
     * @note La funcion solo soporta los formatos JPG, JPEG. Retorna matriz tipo double **
     */
    double **imread(const char *absolutpath, int &width, int &height, double x, double y);

    unsigned char **imread2d(const char *absolutepath, int &width, int &height, int channel = 1);

    StbImageImplementation(/* args */);
    // ~StbImageImplementation();
};

double **StbImageImplementation::stroke(double **image, int &width, int &height, int x, int y)
{
    stb_image stroke = u.initMatrix(width - (2 * x), height - (2 * y), 0.0);

#pragma omp parallel for
    for (int i = y; i < height - y; i++)
    {
        for (size_t j = x; j < width - x; j++)
        {
            stroke[i - y][j - x] = image[i][j];
        }
    }

    u.free_memory(image, height);
    width = width - (2 * x);
    height = height - (2 * y);
    return stroke;
}

double **StbImageImplementation::imresize(double **image, int width, int height, int newWidth, int newHeight)
{
    float *img = new float[width * height];
    float *resize = new float[newWidth * newHeight];
    int cont = 0;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int cont = i * width + j;
            img[cont] = float(image[i][j]);
        }
    }

    stbir_resize_float(img, width, height, 0, resize, newWidth, newHeight, 0, 1);
    u.free_memory(image, height);
    delete[] img;
    return u.vect2mat(resize, (newHeight * newWidth), newWidth, newHeight);
}

stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height)
{
    /*
     * absolutPath: recibe la ruta absoluta de la imagen y el nobre de le imagen
     * Ejemplo: /home/user/img/imagen.jpg
     */
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    stb_image img = u.initMatrix(width, height, 0.0);
    int cont = 0;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int cont = i * width + j;
            *(*(img + i) + j) = double(*(data + cont));
        }
    }

    stbi_image_free(data);

    return img;
}

unsigned char **StbImageImplementation::imread2d(const char *absolutpath, int &width, int &height, int channel)
{
    int n;
    unsigned char *data = stbi_load(absolutpath, &width, &height, &n, 1);
    unsigned char **image = u.initMatrix(width, height, '0');
    int cont = 0;
#pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int cont = i * width + j;
            *(*(image + i) + j) = (*(data + cont));
        }
    }

    stbi_image_free(data);

    return image;
}

stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height, double x, double y)
{
    /*
     * absolutPath: recibe la ruta absoluta de la imagen y el nobre de le imagen
     * Ejemplo: /home/user/img/imagen.jpg
     */
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    stb_image img = this->u.initMatrix(width, height, 0.0);
    int cont = 0;
// printf("Imagen cargada, canales: %d\t, size %d x %d",n, width, height);
#pragma omp parallel for collapse(2)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int cont = i * width + j;
            *(*(img + i) + j) = double(*(data + cont));
        }
    }

    stbi_image_free(data);
    int sx = width * x;
    int sy = height * y;
    img = stroke(img, width, height, sx, sy);
    return img;
}

void StbImageImplementation::imwrite(const char *absolutePath, stb_image image, int width, int height)
{
    char *img = new char[width * height];
    int index = 0;
#pragma omp parallel for collapse(2)
    for (short int i = 0; i < height; i++)
    {
        for (short int j = 0; j < width; j++)
        {
            int index = i * width + j;
            img[index] = (uint8_t)(image[i][j]);
        }
    }

    stbi_write_jpg(absolutePath, width, height, 1, img, 100);
    delete[] img;
}

void StbImageImplementation::imwrite(const char *absolutePath, int **image, int width, int height)
{
    char *img = new char[width * height];
    int index = 0;
#pragma omp parallel for collapse(2)
    for (short int i = 0; i < height; i++)
    {
        for (short int j = 0; j < width; j++)
        {
            int index = i * width + j;
            img[index] = image[i][j] > 1 ? (uint8_t)image[i][j] : static_cast<unsigned char>(image[i][j] * 255);
        }
    }

    stbi_write_jpg(absolutePath, width, height, 1, img, 100);
    delete[] img;
}

void StbImageImplementation::imwrite(const char *absolutePath, unsigned char **image, int width, int height)
{
    char *img = new char[width * height];
    int index = 0;
#pragma omp parallel for collapse(2)
    for (short int i = 0; i < height; i++)
    {
        for (short int j = 0; j < width; j++)
        {
            int index = i * width + j;
            img[index] = image[i][j];
        }
    }

    stbi_write_jpg(absolutePath, width, height, 1, img, 100);
    delete[] img;
}

StbImageImplementation::StbImageImplementation(/* args */)
{
}
