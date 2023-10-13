#include "./classifier/svm-implement/svm-implement.h"
#include "./utility/utility.h"
#include "./utility/metrics.h"
#include "./preprocessing/gaussian_filter/gaussianFilter.h"
#include "./stb-implement/StbImageImplementation.h"
#include "./preprocessing/filter-process/filter-process.h"
#include "./edge_detector/canny/Canny.h"
#include "./edge_detector/sobel/sobel.h"


#define LIB_IS_VERSION 2.0