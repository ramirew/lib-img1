#include <fstream>
#include <vector>
using namespace std;
/**
 * @author Ing. Ismael Farinango - 2023
 * @details Esta clase contiene funciones de uso comun como inicializar matrices, liberar memoria entre otros
 */
struct feature
{
    int label;
    double **imFeature;
    int width;
    int height;
    bool isTrain;
};

class Utility
{
private:
    /* data */
public:
    /**
     * @brief Inicializa matriz nxm
     * @param width NUmero de columnas
     * @param height Numeor de filas
     * @param value Valor con el que se llena la matriz
     */
    int **initMatrix(int width, int height, int value = 0);
    /**
     * @brief Inicializa matriz nxm
     * @param width NUmero de columnas
     * @param height Numeor de filas
     * @param value Valor con el que se llena la matriz
     */
    double **initMatrix(int width, int height, double value = 0.0);
    /**
     * @brief Inicializa matriz nxm
     * @param width NUmero de columnas
     * @param height Numeor de filas
     * @param value Valor con el que se llena la matriz
     */
    unsigned char **initMatrix(int width, int height, char value = 0);
    /**
     * @brief Libera la memoria de una matriz
     * @param matrix Matriz a liberar
     * @param height Numero de filas
     */
    void free_memory(int **matrix, int hedight);
    /**
     * @brief Libera la memoria de una matriz
     * @param matrix Matriz a liberar
     * @param height Numero de filas
     */
    void free_memory(double **matrix, int hedight);
    /**
     * @brief Libera la memoria de una matriz
     * @param matrix Matriz a liberar
     * @param height Numero de filas
     */
    void free_memory(unsigned char **matrix, int hedight);
    /**
     * @brief Escribe el vector de caracteristicas en archivo SCV.
     * @param path Ruta del archivo + el nombre del archivo .csv
     * @param feature Vector de caracteristicas;
     * @param label Etiqueta de la clase
     * @param isTrain Parametro para especificar si el vector de caracteristica es de entrenamiento o test.
     */
    void writeCsvToSVM(const char *path, vector<int> feature, int label, bool isTrain = false);
    void writeCsvToSVM(const char *path, vector<double> feature, int label, bool isTrain = false);
    /**
     * @brief Escribe fichero con el vector de caracteristicas
     * @param path Ruta del fichero, mas el nombre
     * @param feature Objeto que contiene la matriz, las filas y columnas, incluye la etiqueta de la clase.
     */
    void writeCsvToSVM(const char *path, feature feature);
    /**
     * @brief Transforma vectos a matriz
     * @param vect Vector de datos
     * @param width Columnas de la matriz
     * @param height Filas de la matriz
     */
    double **vect2mat(float *vect, int size, int width, int height);

    /**
     * @brief Escala valores de una matriz a un rango de 0 - 255
     * @param matrix matriz que contiene los datos a escalar
     * @param width
     */
    int **scaleMatrix(double **matrix, int width, int height);
    /**
     * @brief Devuelve la suma total de los elementos de un vector.
     * @param vect Vector
     */
    double sum(vector<int> vect);
    /**
     * @brief Escala los valores de ancho y alto de una imagen tomando como referencia el valor maximo.
     * @param width Numero de columnas de la imagen
     * @param height Numero de filas de la imagen
     * @param maxscale Vallor maximo.
     */
    void scale(int &width, int &height, int maxscale);

    void writeLabel(int label);
    /**
     * @brief Escribe las metricas de rendimiento en un fichero
     */
    void writeMetrics(const std::string fileName, std::vector<std::string> content = {}, std::vector<std::string> header = {});

    /**
     * @brief Mensaje de informacion
     */
    void info();

    void writeMatrix(const char *filePath, double **matrix, int width, int height);
    void writeMatrix(const char *filePath, int **matrix, int width, int height);

    /**
     * @brief Retorna el valor maximo de una matriz
     * @param matrix Matriz de datos.
     * @param width Numero de columnas
     * @param height Numero de filas
    */
    double getMaxValue(double ** matrix, int width, int height);
};

double Utility::getMaxValue(double **matrix, int width, int height){
    double maxvalue=matrix[0][0];
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            maxvalue=matrix[i][j]>maxvalue?matrix[i][j]:maxvalue;
        }
        
    }
    return maxvalue;
    
}

void Utility::writeMatrix(const char *filePath, double **matrix, int width, int height)
{
    ofstream file;
    file.open(filePath);
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            file << matrix[i][j] << ", ";
        }
        file << endl;
    }
    file.close();
}

void Utility::writeMatrix(const char *filePath, int **matrix, int width, int height)
{
    ofstream file;
    file.open(filePath);
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            file << matrix[i][j] << ", ";
        }
        file << endl;
    }
    file.close();
}

void Utility::info()
{
    printf("The lib-img library has been successfully integrated.\n\n");
}

void Utility::scale(int &width, int &height, int maxscal)
{
    float p = 0.0;
    if (height > width)
    {
        p = (100 * maxscal) / height;
        height = maxscal;
        width = round(width * (p / 100));
    }
    else if (width > height)
    {
        p = (100 * maxscal) / width;
        width = maxscal;
        height = round(height * (p / 100));
    }
    else if (width == height)
    {
        height = maxscal;
        width = maxscal;
    }
}
double Utility::sum(vector<int> vect)
{
    double sum = 0.0;
    for (int i = 0; i < vect.size(); ++i)
    {
        sum = sum + vect[i];
    }
    return sum;
}
int **Utility::scaleMatrix(double **matrix, int width, int height)
{
    float minValue = matrix[0][0];
    float maxValue = matrix[0][0];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            minValue = (matrix[i][j] < minValue) ? matrix[i][j] : minValue;
            maxValue = (matrix[i][j] > maxValue) ? matrix[i][j] : maxValue;
        }
    }
    int **result = initMatrix(width, height, 0);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            result[i][j] = int((matrix[i][j] - minValue) * 255 / (maxValue - minValue));
        }
    }
    return result;
}

double **Utility::vect2mat(float *vect, int size, int width, int height)
{
    double **matrix = initMatrix(width, height, 0.0);
    int cont = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            matrix[i][j] = double(vect[cont]);
            cont++;
        }
    }
    delete[] vect;
    return matrix;
}

int **Utility::initMatrix(int width, int height, int value)
{
    int **matrix = new int *[height];
    for (int i = 0; i < height; i++)
    {
        matrix[i] = new int[width];
        for (int j = 0; j < width; j++)
        {
            matrix[i][j] = value;
        }
    }
    return matrix;
}

double **Utility::initMatrix(int width, int height, double value)
{
    double **matrix = new double *[height];
    for (int i = 0; i < height; i++)
    {
        matrix[i] = new double[width];
        for (int j = 0; j < width; j++)
        {
            matrix[i][j] = value;
        }
    }
    return matrix;
}

void Utility::free_memory(int **matrix, int height)
{
    for (int i = 0; i < height; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void Utility::free_memory(double **matrix, int height)
{
    for (int i = 0; i < height; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void Utility::free_memory(unsigned char **matrix, int height)
{
    for (int i = 0; i < height; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void Utility::writeLabel(int label)
{
    ofstream file;
    file.open("label.txt", ios_base::app);
    file << label;
    file << endl;
    file.close();
}

void Utility::writeCsvToSVM(const char *path, vector<double> feature, int label, bool isTrain)
{
    ofstream file;
    if (isTrain)
    {
        file.open(path, ios_base::app);
    }
    else
    {
        file.open(path, ios_base::app);
    }
    file << label << "\t";
    for (size_t i = 0; i < feature.size(); i++)
    {
        file << (i + 1) << ":" << feature[i] << "\t";
    }
    file << endl;
    file.close();
}

void Utility::writeCsvToSVM(const char *path, vector<int> feature, int label, bool isTrain)
{
    ofstream file;
    if (isTrain)
    {
        file.open(path, ios_base::app);
    }
    else
    {
        file.open(path, ios_base::app);
    }
    file << label << "\t";
    for (size_t i = 0; i < feature.size(); i++)
    {
        file << (i + 1) << ":" << int(feature[i]) << "\t";
    }
    file << endl;
    file.close();
}
void Utility::writeCsvToSVM(const char *path, feature feature)
{
    ofstream file;
    int cont = 1;
    if (feature.isTrain)
    {
        file.open(path, ios_base::app);
    }
    else
    {
        file.open(path, ios_base::app);
    }
    file << feature.label << "\t";
    for (size_t i = 0; i < feature.width; i++)
    {
        for (size_t j = 0; j < feature.height; j++)
        {
            file << cont << ":" << int(feature.imFeature[j][i]) << "\t";
            cont++;
        }
    }
    file << endl;
    file.close();
}

void Utility::writeMetrics(const std::string fileName, std::vector<std::string> contents, std::vector<std::string> header)
{
    ofstream file;
    file.open(fileName, ios_base::app);
    if (!header.empty())
    {
        for (const auto &head : header)
        {
            file << head << ",";
        }
        file << endl;
    }
    if (!contents.empty())
    {
        for (const auto &content : contents)
        {
            file << content << ",";
        }
        file << endl;
    }
    file.close();
}