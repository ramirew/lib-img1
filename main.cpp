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
    HistogramHandler h;
    MediaFilter m;
    int width, height;
    double** image=stb.imread("/home/ismael/Documents/cara.jpg",width,height);
    stb.imwrite("org.jpg",image,width,height);
    int**med=m.medianFilter(image,width,height);
    stb.imwrite("img.jpeg",med,width,height);
    u.free_memory(med,height);
    u.free_memory(image,height);
    return 0;
}