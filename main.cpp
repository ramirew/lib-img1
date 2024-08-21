#include "./lib-img/lib/lib-include.h"
#include <chrono>
#include <sys/resource.h>
#include <sys/time.h>
#include <iostream>

bool compareMatrices(double** matrix1, double** matrix2, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (fabs(matrix1[i][j] - matrix2[i][j])) { // poner vien la condicion hp
                return true;
            }
        }
    }
    return false;
}
double** calculateDifference(double** matrix1, double** matrix2, int width, int height) { //esto esta bien 
    double** difference = new double*[height];
    for (int i = 0; i < height; i++) {
        difference[i] = new double[width];
        for (int j = 0; j < width; j++) {
            difference[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }
    return difference;
}

int main() {
    StbImageImplementation stb;
    Utility u;
    GaussianFilter g;
    int width, height;
    const char* imagePath = "/home/mickel/Downloads/examen-paralela-master/imageGasSecuencial.jpg"; // Define la ruta de la imagen a leer
    const char* imagePath2 = "/home/mickel/Downloads/examen-paralela-master/imageGasParalela.jpg"; // Define la ruta de la imagen a leer
    // Lectura de la imagen
    double **image = stb.imread(imagePath, width, height);
     double **image2 = stb.imread(imagePath2, width, height);
    if (image == nullptr) {
        std::cerr << "Error al cargar la imagen." << std::endl;
        return -1;
    }
     double **difference = calculateDifference(image, image2, width, height);
     std::cout << "Diferencia entre matrices:" << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << difference[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // Aplicación de filtro gaussiano
    double sigma = 1; // Valor de sigma ajustable
    double **filteredImage = g.gaussianFilter(image, width, height, sigma);

    // Imprimir la matriz resultante
    
    bool areEqual = compareMatrices(image2, image, width, height);
      
    std::cout << "Las matrices son " << (areEqual ? "iguales." : "diferentes.") << std::endl;
  
    // Guardar imagen procesada
    //stb.imwrite("/home/matheo/Descargas/SecuencialIamgenesParalela (1)/v4.jpg", filteredImage, width, height);

    // Liberación de memoria
    u.free_memory(filteredImage, height); // Asegúrate de que esta función libera adecuadamente la memoria

    return 0;
}
  
