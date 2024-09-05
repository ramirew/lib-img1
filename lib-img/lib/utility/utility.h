#pragma once

#include <fstream>
#include <vector>
#include <omp.h>
using namespace std;
/**
 * @author Ing. Ismael Farinango - 2023
 * @details Esta clase contiene funciones de uso común como inicializar matrices, liberar memoria, entre otros.
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
    int **initMatrix(int width, int height, int value = 0);
    double **initMatrix(int width, int height, double value = 0.0);
    unsigned char **initMatrix(int width, int height, char value = 0);
    void free_memory(int **matrix, int height);
    void free_memory(double **matrix, int height);
    void free_memory(unsigned char **matrix, int height);
    void writeCsvToSVM(const char *path, vector<int> feature, int label, bool isTrain = false);
    void writeCsvToSVM(const char *path, vector<double> feature, int label, bool isTrain = false);
    void writeCsvToSVM(const char *path, feature feature);
    double **vect2mat(float *vect, int size, int width, int height);
    int **scaleMatrix(double **matrix, int width, int height);
    double sum(vector<int> vect);
    void scale(int &width, int &height, int maxscale);
    void writeLabel(int label);
    void writeMetrics(const std::string fileName, std::vector<std::string> content = {}, std::vector<std::string> header = {});
    void info();
    void writeMatrix(const char *filePath, double **matrix, int width, int height);
    void writeMatrix(const char *filePath, int **matrix, int width, int height);
    double getMaxValue(double **matrix, int width, int height);
    double **int2double(int **matrix, int width, int height);
    int **double2int(double **matrix, int width, int height);
};

double **Utility::int2double(int **matrix, int width, int height)
{
    double **newMatrix = initMatrix(width, height, 0.0);
#pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            newMatrix[i][j] = static_cast<double>(matrix[i][j]);
        }
    }
    free_memory(matrix, height);
    return newMatrix;
}

int **Utility::double2int(double **matrix, int width, int height)
{
    int **newMatrix = initMatrix(width, height, 0);
#pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            newMatrix[i][j] = static_cast<int>(matrix[i][j]);
        }
    }
    free_memory(matrix, height);
    return newMatrix;
}

double Utility::getMaxValue(double **matrix, int width, int height)
{
    double maxvalue = matrix[0][0];
#pragma omp parallel for reduction(max : maxvalue)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (matrix[i][j] > maxvalue)
            {
                maxvalue = matrix[i][j];
            }
        }
    }
    return maxvalue;
}

void Utility::writeMatrix(const char *filePath, double **matrix, int width, int height)
{
    ofstream file;
    file.open(filePath);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
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
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
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

void Utility::scale(int &width, int &height, int maxscale)
{
    float p = 0.0;
    if (height > width)
    {
        p = (100 * maxscale) / height;
        height = maxscale;
        width = round(width * (p / 100));
    }
    else if (width > height)
    {
        p = (100 * maxscale) / width;
        width = maxscale;
        height = round(height * (p / 100));
    }
    else if (width == height)
    {
        height = maxscale;
        width = maxscale;
    }
}

double Utility::sum(vector<int> vect)
{
    double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < vect.size(); ++i)
    {
        sum += vect[i];
    }
    return sum;
}

int **Utility::scaleMatrix(double **matrix, int width, int height)
{
    float minValue = matrix[0][0];
    float maxValue = matrix[0][0];
#pragma omp parallel for reduction(min : minValue) reduction(max : maxValue)
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            minValue = (matrix[i][j] < minValue) ? matrix[i][j] : minValue;
            maxValue = (matrix[i][j] > maxValue) ? matrix[i][j] : maxValue;
        }
    }
    int **result = initMatrix(width, height, 0);
#pragma omp parallel for
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
#pragma omp parallel for collapse(2)
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
#pragma omp parallel for
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
#pragma omp parallel for
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

unsigned char **Utility::initMatrix(int width, int height, char value)
{
    unsigned char **matrix = new unsigned char *[height];
#pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        matrix[i] = new unsigned char[width];
        for (int j = 0; j < width; j++)
        {
            matrix[i][j] = value;
        }
    }
    return matrix;
}

void Utility::free_memory(int **matrix, int height)
{
#pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void Utility::free_memory(double **matrix, int height)
{
#pragma omp parallel for
    for (int i = 0; i < height; i++)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void Utility::free_memory(unsigned char **matrix, int height)
{
#pragma omp parallel for
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
