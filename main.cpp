#include "./lib/lib-include.h" // add to project
int main(){
    /**
     * This library have:
     * Canny
     * GaussianFilter
     * Sobel
     * svm implement
     * stb_image implement
    */
    //Example to call any class
    StbImageImplementation stb;
    Utility u;
    GaussianFilter g;
    Canny c;
    Sobel s;
    int width, height;
    double** image=stb.imread("/home/ismael/Documents/cara.jpg",width,height);
    double thresh[2]={0.2,0.5};
    int **sobel=s.applySobel(image,width,height);
    stb.imwrite("img.jpeg",sobel,width,height);
    u.free_memory(sobel,height);
    return 0;
}