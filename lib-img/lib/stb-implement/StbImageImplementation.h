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

/**
 * @brief Realiza un trazo en la imagen dada, eliminando los bordes externos.
 *
 * @param image La imagen original.
 * @param width La anchura de la imagen original.
 * @param height La altura de la imagen original.
 * @param x El tamaño del borde a eliminar en el eje x.
 * @param y El tamaño del borde a eliminar en el eje y.
 * @return double** La imagen con los bordes eliminados.
 */
double **StbImageImplementation::stroke(double **image, int &width, int &height, int x, int y)
{
    // Inicialización de la matriz 'stroke' con las nuevas dimensiones
    stb_image stroke = u.initMatrix(width - (2 * x), height - (2 * y), 0.0);

// Paralelización de los bucles utilizando OpenMP
#pragma omp parallel for collapse(2)
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

/**
 * @brief Redimensiona una imagen representada como una matriz de valores de tipo double.
 *
 * @param image La imagen original representada como una matriz de valores de tipo double.
 * @param width El ancho de la imagen original.
 * @param height La altura de la imagen original.
 * @param newWidth El nuevo ancho deseado para la imagen redimensionada.
 * @param newHeight La nueva altura deseada para la imagen redimensionada.
 * @return double** La imagen redimensionada representada como una matriz de valores de tipo double.
 */
double **StbImageImplementation::imresize(double **image, int width, int height, int newWidth, int newHeight)
{
    float *img = new float[width * height];
    float *resize = new float[newWidth * newHeight];
    int cont = 0;

#pragma omp parallel for collapse(2) // Utiliza collapse(2) para paralelizar ambos bucles
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = i * width + j;
            img[index] = float(image[i][j]);
        }
    }

    stbir_resize_float(img, width, height, 0, resize, newWidth, newHeight, 0, 1);
    u.free_memory(image, height);
    delete[] img;

    return u.vect2mat(resize, (newHeight * newWidth), newWidth, newHeight);
}

/**
 * Lee una imagen desde una ruta absoluta y devuelve una matriz de píxeles.
 *
 * @param absolutPath La ruta absoluta de la imagen.
 * @param width La anchura de la imagen (se actualiza con el valor real).
 * @param height La altura de la imagen (se actualiza con el valor real).
 * @return Una matriz de píxeles representando la imagen.
 */
stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height)
{
    /*
     * absolutPath: recibe la ruta absoluta de la imagen y el nombre de la imagen
     * Ejemplo: /home/user/img/imagen.jpg
     */
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    if (data == nullptr)
    {
        std::cerr << "Error: no se pudo cargar la imagen." << std::endl;
        exit(1);
    }

    stb_image img = u.initMatrix(width, height, 0.0);

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

/**
 * Lee una imagen en formato 2D desde un archivo y la almacena en una matriz de píxeles.
 *
 * @param absolutpath Ruta absoluta del archivo de imagen.
 * @param width Referencia a la variable que almacenará el ancho de la imagen.
 * @param height Referencia a la variable que almacenará la altura de la imagen.
 * @param channel Número de canales de color de la imagen.
 * @return Puntero a una matriz de píxeles que representa la imagen.
 */
unsigned char **StbImageImplementation::imread2d(const char *absolutpath, int &width, int &height, int channel)
{
    int n;
    unsigned char *data = stbi_load(absolutpath, &width, &height, &n, channel);
    if (data == nullptr)
    {
        std::cerr << "Error: no se pudo cargar la imagen." << std::endl;
        exit(1);
    }

    unsigned char **image = u.initMatrix(width, height, '0');

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

/**
 * Carga una imagen desde una ruta absoluta y la convierte en una matriz de valores de píxeles.
 *
 * @param absolutPath La ruta absoluta de la imagen y el nombre de la imagen.
 *                    Ejemplo: /home/user/img/imagen.jpg
 * @param width       La variable de salida que almacenará el ancho de la imagen cargada.
 * @param height      La variable de salida que almacenará la altura de la imagen cargada.
 * @param x           El factor de escala horizontal para redimensionar la imagen.
 * @param y           El factor de escala vertical para redimensionar la imagen.
 * @return            La matriz de valores de píxeles de la imagen cargada.
 */
stb_image StbImageImplementation::imread(const char *absolutPath, int &width, int &height, double x, double y)
{
    /*
     * absolutPath: recibe la ruta absoluta de la imagen y el nombre de la imagen
     * Ejemplo: /home/user/img/imagen.jpg
     */
    int n;
    unsigned char *data = stbi_load(absolutPath, &width, &height, &n, 1);
    if (data == nullptr)
    {
        std::cerr << "Error: no se pudo cargar la imagen." << std::endl;
        exit(1);
    }

    stb_image img = this->u.initMatrix(width, height, 0.0);

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

/**
 * @brief Guarda una imagen en formato JPG en la ruta especificada.
 *
 * @param absolutePath Ruta absoluta donde se guardará la imagen.
 * @param image Matriz de píxeles de la imagen.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 */
void StbImageImplementation::imwrite(const char *absolutePath, stb_image image, int width, int height)
{
    char *img = new char[width * height];

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

/**
 * Guarda una imagen en formato JPG en la ruta especificada.
 *
 * @param absolutePath La ruta absoluta donde se guardará la imagen.
 * @param image El puntero a la matriz de enteros que representa la imagen.
 * @param width El ancho de la imagen.
 * @param height La altura de la imagen.
 */
void StbImageImplementation::imwrite(const char *absolutePath, int **image, int width, int height)
{
    char *img = new char[width * height];

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

/**
 * Guarda una imagen en formato JPG en la ruta especificada.
 *
 * @param absolutePath La ruta absoluta donde se guardará la imagen.
 * @param image El puntero a la matriz de píxeles de la imagen.
 * @param width El ancho de la imagen.
 * @param height La altura de la imagen.
 */
void StbImageImplementation::imwrite(const char *absolutePath, unsigned char **image, int width, int height)
{
    char *img = new char[width * height];
    int index = 0;
#pragma omp parallel for collapse(2)
    for (short int i = 0; i < height; i++)
    {
        for (short int j = 0; j < width; j++)
        {
            img[index] = image[i][j];
            index++;
        }
    }
    stbi_write_jpg(absolutePath, width, height, 1, img, 100);
    delete[] img;
}

StbImageImplementation::StbImageImplementation(/* args */)
{
}
