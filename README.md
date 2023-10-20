# lib-img
Library to process images, have Canny, Sobel, Gaussian filter.
# Instalar
  1. **Crear nuevo proyecto**<br/>
     Crear un nuevo directorio.
     ```bash
     $ sudo mkdir mi_proyecto
     ```
     Abrir Visual Studio code y abrir el directorio que acaba de crear.
     
  2. **Clonar repositorio**<br/>
     Abrir la terminal de Visual Studio y clonar el repositorio.<br/>
     ```bash 
       $ git clone https://github.com/ismaelfari1996/lib-img.git
     ```
     Crear un fichero en main.cpp dentro del proyecto.
     ```bash
       -mi_proyecto/
           |-lib-img/
           |-main.cpp
     ```
      Agregar libreria en <b>main.cpp</b>.<br/>
      ```bash
        #include "./lib-img/lib/lib-include.h
      ```
# leer imagenes
Lib-img contiene la implementación de STB_IMAGE.La clase StbImageImplementation contiene dos funciones: imread y imwrite. 
A contunuación se muestra un ejemplo de lectura y escritura de imagen.
```bash
#include "./lib-img/lib/lib-include.h
int main(){
Utility u;
StbImageImplementation stb;
int width, height;
double** image=stb.imread("ruta_imagen/imagen.jpg",width, height); //Lectura de imagen
//Guardar imagen
stb.imwrite("image.jpg",image,width, height);
//Liberar puntero
 u.free_memory(image,height);
}
```
<b>Nota:</b> Solo soporta imágenes de formato JPG, JPEG.
# Preprocesamiento de imagenes
  1. **Filtro Gaussiano**<br/>
```bash
#include "./lib-img/lib/lib-include.h
int main(){
Utility u;
StbImageImplementation stb;
GassianFilter  g;
int width, height;
double** image=stb.imread("ruta_imagen/imagen.jpg",width, height); //Lectura de imagen
double sigma=1; //Valor ajustable
image=g.gaussianFilter(image,width,height,sigma); // Aplica filtro gaussiano
//Guardar imagen
stb.imwrite("image.jpg",image,width, height);
//Liberar puntero
 u.free_memory(image,height);
}
```
2. **Equalizacion de histograma**<br/>
```bash
#include "./lib-img/lib/lib-include.h
int main(){
Utility u;
StbImageImplementation stb;
HistogramHandler h; //instancia a la clase histograma
int width, height;
double** image=stb.imread("ruta_imagen/imagen.jpg",width, height); //Lectura de imagen
int ** histeq=h.histeq(image,width,height);//equaliza el histograma de la imagen
//Guardar imagen
stb.imwrite("image.jpg",histeq,width, height);
//Liberar puntero
 u.free_memory(histeq,height);
}
```
