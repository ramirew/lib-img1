#include <vector>
#include <omp.h> // Asegúrate de incluir la librería de OpenMP

class FilterProcess
{
private:
    Utility u; // Clase Utility que probablemente contiene métodos auxiliares para manejar matrices.

public:
    FilterProcess(/* args */);

    // Función que realiza la convolución de un píxel en la imagen usando un kernel.
    // Parámetros:
    // - image: Referencia a un puntero doble que apunta a la imagen en escala de grises.
    // - positionX: Posición X (columna) del píxel sobre el que se aplicará la convolución.
    // - positionY: Posición Y (fila) del píxel sobre el que se aplicará la convolución.
    // - kernel: Kernel de convolución representado como una matriz 2D de enteros.
    // - kernelSize: Tamaño del kernel (dimensiones nxn).
    double convolution(double **&image, short int positionX, short int positionY, double **kernel, short int kernelSize);

    // Sobrecarga de la función de convolución, usando un kernel de tipo std::vector.
    // Parámetros:
    // - image: Referencia a un puntero doble que apunta a la imagen en escala de grises.
    // - positionX: Posición X (columna) del píxel sobre el que se aplicará la convolución.
    // - positionY: Posición Y (fila) del píxel sobre el que se aplicará la convolución.
    // - kernel: Kernel de convolución representado como un std::vector 2D de enteros.
    double convolution(double **&image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel);

    // Función que aplica padding de ceros a una imagen para que sea compatible con un kernel.
    // Parámetros:
    // - image: Referencia a un puntero doble que apunta a la imagen en escala de grises.
    // - width: Referencia al ancho de la imagen (se actualizará con el nuevo ancho después del padding).
    // - height: Referencia al alto de la imagen (se actualizará con el nuevo alto después del padding).
    // - kernelSize: Tamaño del kernel que se usará para la convolución.
    double **zeroPadding(double **&image, short int &width, short int &height, short int kernelSize);

    // Función que aplica la convolución 2D a una imagen usando un kernel representado como un std::vector de enteros.
    // Parámetros:
    // - image: Puntero doble a la imagen en escala de grises.
    // - width: Ancho de la imagen (número de columnas).
    // - height: Alto de la imagen (número de filas).
    // - kernel: Kernel de convolución representado como un std::vector 2D de enteros.
    double **conv2d(double **image, short int width, short int height, std::vector<std::vector<int>> kernel);

    // Sobrecarga de la función conv2d que aplica la convolución 2D usando un kernel representado como un std::vector de dobles.
    // Parámetros:
    // - image: Puntero doble a la imagen en escala de grises.
    // - width: Ancho de la imagen (número de columnas).
    // - height: Alto de la imagen (número de filas).
    // - kernel: Kernel de convolución representado como un std::vector 2D de dobles.
    int **conv2d(double **image, short int width, short int height, std::vector<std::vector<double>> kernel);
};

FilterProcess::FilterProcess(/* args */)
{
    // Constructor vacío, puede inicializar algo si es necesario.
}

// Implementación de la función zeroPadding
double **FilterProcess::zeroPadding(double **&image, short int &width, short int &height, short int kernelSize)
{
    short int paddingSize = (kernelSize / 2) * 2; // Tamaño del padding a agregar.
    short int index = kernelSize / 2; // Índice inicial para ajustar la posición en la imagen con padding.
    auto **imagePadding = u.initMatrix((width + paddingSize), (height + paddingSize), 0.0); // Inicializa la imagen con padding.

#pragma omp parallel for collapse(2)
    for (short int i = index; i < (height + index); i++)
    {
        for (short int j = index; j < (width + index); j++)
        {
            imagePadding[i][j] = image[i - index][j - index]; // Copia los valores de la imagen original a la imagen con padding.
        }
    }

    width = (width + paddingSize); // Actualiza el ancho de la imagen con padding.
    height = (height + paddingSize); // Actualiza el alto de la imagen con padding.
    return imagePadding; // Devuelve la imagen con padding.
}

// Implementación de la función de convolución con un kernel de tipo std::vector
double FilterProcess::convolution(double **&image, short int positionX, short int positionY, std::vector<std::vector<int>> kernel)
{
    double sum = 0.0;
    int kernelRow = 0, kernelCol = 0;
    for (int row = positionY; row < (kernel.size() + positionY); row++)
    {
        for (int col = positionX; col < (kernel.size() + positionX); col++)
        {
            sum += (image[row][col] * kernel[kernelRow][kernelCol]); // Calcula la suma ponderada de la convolución.
            kernelCol++;
        }
        kernelCol = 0;
        kernelRow++;
    }

    return sum; // Devuelve el resultado de la convolución.
}

// Implementación de la función de convolución con un kernel de tipo puntero doble
double FilterProcess::convolution(double **&image, short int positionX, short int positionY, double **gaussianKernel, short int kernelSize)
{
    double sum = 0.0;
    int gaussianKernelRow = 0, gaussianKernelCol = 0;
    for (int row = positionY; row < (kernelSize + positionY); row++)
    {
        for (int col = positionX; col < (kernelSize + positionX); col++)
        {
            sum += (image[row][col] * gaussianKernel[gaussianKernelRow][gaussianKernelCol]); // Calcula la suma ponderada de la convolución.
            gaussianKernelCol++;
        }
        gaussianKernelCol = 0;
        gaussianKernelRow++;
    }

    return sum; // Devuelve el resultado de la convolución.
}

// Implementación de la función conv2d con un kernel de tipo std::vector
double **FilterProcess::conv2d(double **image, short int width, short int height, std::vector<std::vector<int>> kernel)
{
    double **result = u.initMatrix(width, height, 0.0); // Inicializa la matriz resultado.
    double **matrixPadding = zeroPadding(image, width, height, kernel.size()); // Aplica padding a la imagen.
    short int row = height - kernel.size();
    short int col = width - kernel.size();

#pragma omp parallel for collapse(2)
    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernel); // Aplica la convolución y guarda el resultado.
        }
    }

    u.free_memory(matrixPadding, height); // Libera la memoria utilizada por la imagen con padding.
    return result; // Devuelve la imagen resultante después de la convolución.
}

// Implementación de la función conv2d con un kernel de tipo std::vector de dobles
int **FilterProcess::conv2d(double **image, short int width, short int height, std::vector<std::vector<double>> kernel)
{
    int **result = u.initMatrix(width, height, 0); // Inicializa la matriz resultado.
    double **matrixPadding = zeroPadding(image, width, height, kernel.size()); // Aplica padding a la imagen.
    short int row = height - kernel.size();
    short int col = width - kernel.size();
    double **kernelCopy = u.initMatrix(kernel.size(), kernel.size(), 0.0); // Copia del kernel para usar con OpenMP.

#pragma omp parallel for collapse(2)
    for (int i = 0; i < kernel.size(); i++)
    {
        for (int j = 0; j < kernel.size(); j++)
        {
            kernelCopy[i][j] = kernel[i][j]; // Copia el kernel original.
        }
    }

    for (short int y = 0; y <= row; y++)
    {
        for (short int x = 0; x <= col; x++)
        {
            result[y][x] = convolution(matrixPadding, x, y, kernelCopy, kernel.size()); // Aplica la convolución y guarda el resultado.
        }
    }

    u.free_memory(matrixPadding, height); // Libera la memoria utilizada por la imagen con padding.
    u.free_memory(kernelCopy, kernel.size()); // Libera la memoria utilizada por la copia del kernel.
    return result; // Devuelve la imagen resultante después de la convolución.
}
