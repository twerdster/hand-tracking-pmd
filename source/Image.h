#include "mex.h"
#include <math.h>
//typedef int[3] PICCORDS; //stands for picture cords, a 3-vector of type (x,y,1)^t
//typedef double[3] REALCORDS;
#ifndef IMAGE_H_
#define IMAGE_H_
#define NAN sqrt(-1.0)

namespace HT {
    class Image {
    public:
        int M;
        int N;
        double* image;
        Image() { this->M = 0; this->N = 0; this->image = 0; }
        Image(double* image, int M, int N);
        Image(const Image& A);
        ~Image();
        Image& operator=(const Image& A);
        Image getCircumference() const;
        double* getReal(const int* picCords) const;
    };
}

#endif