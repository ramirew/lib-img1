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
Lib-img contiene la implementación de STB_IMAGE.La clase StbImageImplementation contiene dos funciones: imread y imwrite. La función <b> imread</b> recibe tres parámetros:
<br/>
<b>ruta_imagen/image.jpg</b> de tipo const char*
<br/>
<b>width</b> de tipo int
<br/>
<b>height</b> de tipo int
<br/>
Para guardar una imágen puede hacer uso de la función <b>imwrite</b>. Los parámetros requeridos son:
<br/>
<b>ruta_imagen/imagen.jpg</b> de tipo const char*
<br/>
<b>width</b> de tipo int
<br/>
<b>height</b> de tipo int
<br/>
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
