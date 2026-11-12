#include "mex.h"
#include "Image.h"


#ifndef HAND_H_
#define HAND_H_
namespace HT {
   
    
    class Hand {
        public:
            int ID;
            double curCMReal[3];
            double prevCMReal[3];
            Image depthImage;
            Image segmentedImage;
            double* realCordsMap;
            int inHandPixels;
            int* seed;
            int parture() const;
            Hand();          
            Hand(const Hand& hand);
            Hand(int ID, double* curCMReal, double* prevCMReal, const Image& DepthImage);
            ~Hand();
            Hand& operator=(Hand& hand);
            double getDistanceBetween(int picI1, int picJ1, int picI2, int picJ2) const;
            double* F1velocityAprox() const;
            int* F2seed() const;
            Image F3segmentate();
            int* F4boundary() const;
            double* F5mean() const;
            int* F6nextCenterOfMass() const;
};
}
#endif