#include <vector>
#include <omp.h>

class HistogramHandler
{
private:
    Utility u; // Clase Utility que probablemente tiene métodos auxiliares para manipular matrices.

public:
    // Función que calcula el histograma de una imagen.
    // Parámetros:
    // - image: Puntero doble a una matriz 2D que representa la imagen en escala de grises.
    // - width: Ancho de la imagen (número de columnas).
    // - height: Alto de la imagen (número de filas).
    std::vector<int> imhist(double **image, int width, int height);

    // Función que realiza la ecualización del histograma de una imagen.
    // Parámetros:
    // - image: Puntero doble a una matriz 2D que representa la imagen en escala de grises.
    // - width: Ancho de la imagen (número de columnas).
    // - height: Alto de la imagen (número de filas).
    int **histeq(double **image, int width, int height);
};

// Implementación de la función imhist
std::vector<int> HistogramHandler::imhist(double **image, int width, int height)
{
    std::vector<int> hist(256, 0); // Histograma de la imagen, inicializado en 0.

#pragma omp parallel for collapse(2) shared(hist, image)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
#pragma omp atomic
            hist[image[i][j]]++; // Incrementa el valor del histograma en la posición correspondiente al valor del píxel.
        }
    }

    return hist; // Devuelve el histograma calculado.
}

// Implementación de la función histeq
int **HistogramHandler::histeq(double **image, int width, int height)
{
    int **imageEq = u.initMatrix(width, height, 0); // Matriz para la imagen ecualizada, inicializada en 0.
    std::vector<int> histogram = imhist(image, width, height); // Calcula el histograma de la imagen.
    int pixels = width * height; // Número total de píxeles en la imagen.
    int cdf[256] = {0}; // Función de distribución acumulada (CDF), inicializada en 0.
    double sumCDF = 0;

#pragma omp parallel
    {
        double sumCDF_local = 0.0;

#pragma omp for
        for (int i = 0; i < 256; i++)
        {
            sumCDF_local += static_cast<double>(histogram[i]) / static_cast<double>(pixels);
            cdf[i] = sumCDF_local * 255.0; // Escala la CDF para el rango [0, 255].
        }

#pragma omp critical
        {
            for (int i = 0; i < 256; i++)
            {
                sumCDF += static_cast<double>(histogram[i]) / static_cast<double>(pixels);
                cdf[i] = sumCDF * 255.0; // Escala la CDF para el rango [0, 255].
            }
        }
    }

#pragma omp parallel for collapse(2) shared(imageEq, image, cdf)
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            imageEq[i][j] = cdf[static_cast<int>(image[i][j])]; // Aplica la ecualización del histograma.
        }
    }

    u.free_memory(image, height); // Libera la memoria de la imagen original.
    return imageEq; // Devuelve la imagen ecualizada.
}

