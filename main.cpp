#include "./lib/lib-include.h" // add to project
int main()
{
    Utility u;
    StbImageImplementation stb;
    HistogramHandler h; // instancia a la clase histograma
    int width, height;
    double **image = stb.imread("/home/xriva20/Documentos/examen-paralela/ImagensPrueba/IMG5000.jpg", width, height); // Lectura de imagen
    int **histeq = h.histeq(image, width, height);                                                                    // equaliza el histograma de la imagen
    // Guardar imagen
    stb.imwrite("image.jpg", histeq, width, height);
    // Liberar puntero
    u.free_memory(histeq, height);
}