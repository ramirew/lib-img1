#include "./lib-img/lib/lib-include.h"
#include <chrono>
#include <sys/resource.h>
#include <iostream>

using namespace std;
using namespace std::chrono;

int main()
{
    // Variables para medición de tiempo
    auto start = high_resolution_clock::now();

    // Variables para uso de memoria
    struct rusage usage;

    // Código original
    Utility u;
    StbImageImplementation stb;
    HistogramHandler h; // instancia a la clase histograma
    int width, height;
    double **image = stb.imread("/home/xriva20/Documentos/Proyectogit/ImagenesParalela/prueba2.jpg", width, height); // Lectura de imagen
    int **histeq = h.histeq(image, width, height);                                                                   // equaliza el histograma de la imagen
    // Guardar imagen
    stb.imwrite("histograma.jpg", histeq, width, height);
    // Liberar puntero
    u.free_memory(histeq, height);

    // Fin del código original
    auto end = high_resolution_clock::now();

    // Calcular tiempos
    auto duration = duration_cast<microseconds>(end - start).count();

    // Obtener uso de recursos
    getrusage(RUSAGE_SELF, &usage);

    cout << "Tiempo real (microsegundos): " << duration << endl;
    cout << "Tiempo de uso del CPU (segundos): " << (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) << endl;
    cout << "Uso de memoria máxima (kilobytes): " << usage.ru_maxrss << endl;

    return 0;
}