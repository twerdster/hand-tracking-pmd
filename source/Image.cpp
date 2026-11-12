#include "Image.h"
#include "mex.h"
#include "stdlib.h"
#include <string.h>
#include <math.h>

#define CIRCTHRESH 0.07
#define OUTERPOINT -1
#define BOUNDARY 0

using namespace HT;

static bool isnan(double var) {
    volatile double d = var;
    return d != d;
}

Image::Image(double* image, int M, int N) {
    if((M <= 0) || (N <= 0) || (image == 0)) {
        // error handling.
    }
    this->M = M;
    this->N = N;
    this->image = new double[M*N];
    // should I verify this alocation?
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < N; j++) {
            this->image[i + M*j] = double(image[i + M*j]);
        }
    }
}

Image& Image::operator=(const Image& A) {
    if((A.M == 0) || (A.N == 0) || (A.image == 0)) {
        // error handling;
    }
    if(this->M*this->N > 0) {                                                           // fix.
        this->M = 0;
        delete [] this->image;
        this->image = 0;
    }
    this->M = A.M;
    this->N = A.N;
    this->image = new double[A.M*A.N];
    // add a check to see that it worked?
    for(int i = 0; i < A.M; i++) {
        for(int j = 0; j < A.N; j++) {
            this->image[i+A.M*j] = A.image[i+A.M*j];
        }
    }
    return *this;
}

Image::Image(const Image& A) {
    if((A.M == 0) || (A.N == 0) || (A.image == 0)) {
        //error handling if the image I recieved is invalid
    }
    this->M = A.M;
    this->N = A.N;
    this->image = new double[A.M*A.N];
    // add a try catch for allocation errors
    for(int i = 0; i < A.M; i++) {
        for(int j = 0; j < A.N; j++) {
            this->image[i+A.M*j] = A.image[i+A.M*j];
        }
    }
}

Image::~Image() {
    if(!this || !this->image) {
        return;
    }
    delete [] this->image;
    this->image = 0;
}



Image Image::getCircumference() const {
    double *circumference = new double[this->M*this->N];
    for(int i = 0; i < this->M; i++) {
        for(int j = 0; j < this->N; j++) {
            circumference[i+M*j] = OUTERPOINT;
            if(isnan(this->image[i+M*j])) {
                continue;
            }
            if((i < this->M-1) && (isnan(this->image[(i+1)+M*j]) || (fabs(this->image[i+M*j] - this->image[(i+1)+M*j]) > CIRCTHRESH))) {
                circumference[i+M*j] = BOUNDARY;
                continue;
            }
             if((i > 0) &&  (isnan(this->image[(i-1)+M*j]) || (fabs(this->image[i+M*j] - this->image[(i-1)+M*j]) > CIRCTHRESH))) {
                circumference[i+M*j] = BOUNDARY;
                continue;
            }
             if((j < this->N-1) && (isnan(this->image[i+M*(j+1)]) || (fabs(this->image[i+M*j] - this->image[i+M*(j+1)]) > CIRCTHRESH))) {
                circumference[i+M*j] = BOUNDARY;
                continue;
            }
             if((j > 0) &&  (isnan(this->image[i+M*(j-1)]) || (fabs(this->image[i+M*j] - this->image[i+M*(j-1)]) > CIRCTHRESH))) {
                circumference[i+M*j] = BOUNDARY;
                continue;
            }
        }
    }
    Image newImage = Image(circumference, this->M, this->N);
    delete [] circumference;
    return newImage;
}
 

double* Image::getReal(const int* picCords) const {
    double* realCords = new double[3];
    //the K matrix multiplication K = [fx 0 cx; 0 fy cy; 0 0 1]
    // Z*(u,v,1)^t = K*(X,Y,Z)^t
    //double K[3][3] = {{104.119, 0, 81.9494},              // 90, 90 <----
    //                                     {0, 103.588, 59.4392},
    //                                     {0, 0, 1}};
    double Kinv[3][3] = {{0.0096, 0, -0.7871}, 
                         {0, 0.0097, -0.5738},
                         {0, 0, 1}};
    if((picCords[0] < 0) || (picCords[0] >= this->M) || (picCords[1] < 0) || 
           (picCords[1] >= this->N) || (picCords[2] != 1)) {
        // should add a specific error
        realCords[0] = NAN;
        realCords[1] = NAN;
        realCords[2] = NAN;
        return realCords;
    }
   double Z = this->image[picCords[0] + this->M*picCords[1]];
   if(isnan(Z)) {              // in case the indices are in range but the point's depth value is NAN
        realCords[0] = NAN;
        realCords[1] = NAN;
        realCords[2] = NAN;
        return realCords;
   }
   realCords[0] = double(Z*(Kinv[0][0]*(picCords[0]) + Kinv[0][1]*(picCords[1]) + Kinv[0][2]*picCords[2]));
   realCords[1] = double(Z*(Kinv[1][0]*(picCords[0]) + Kinv[1][1]*(picCords[1]) + Kinv[1][2]*picCords[2]));
   realCords[2] = double(Z*(Kinv[2][0]*(picCords[0]) + Kinv[2][1]*(picCords[1]) + Kinv[2][2]*picCords[2]));
   return realCords;
}