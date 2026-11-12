#include "mex.h"
#include "Hand.h"
#include "Image.h"
#include "Queue.h"
#include <string.h>
#include <math.h>
#include <time.h>

#define WINDOW_M 232.94502
#define WINDOW_N 20.82528
#define SEEDINFNORM 1000000
#define MAPSL_DEFAULT_VALUE 101
#define OUTERPOINT -1
#define BOUNDARY 0
#define INNERPOINT 1
#define JUMPTHRESH 0.3
#define MAXDISTANCE 0.25
#define MAXBNDRYDIST 0.08
#define MEANTHRESH 0.2


using namespace HT;

static bool isnan(double var) {
    volatile double d = var;
    return d != d;
}

static double getNorm(double* vector) {
    // add a check that this really is a valid vector.
    return sqrt(vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2]);
}


int Hand::parture() const {
    if((isnan(curCMReal[2])) || (curCMReal[2] <= 0) || (int(1/curCMReal[2]) <=1)) {
        return 1;
    }
    return int(1/curCMReal[2]);
}


Hand::Hand() {
    ID = 0;
    curCMReal[0] = curCMReal[1] = curCMReal[2] = 0;
    prevCMReal[0] = prevCMReal[1] = prevCMReal[2] = 0;
    depthImage = segmentedImage = Image();
    realCordsMap = 0;
    inHandPixels = 0;
    seed = 0;
}
    
Hand::Hand(const Hand& hand) {
    if(!this) {
        return;
    }
    ID = hand.ID;
    curCMReal[0] = hand.curCMReal[0];
    curCMReal[1] = hand.curCMReal[1];
    curCMReal[2] = hand.curCMReal[2];
    prevCMReal[0] = hand.prevCMReal[0];
    prevCMReal[1] = hand.prevCMReal[1];
    prevCMReal[2] = hand.prevCMReal[2];
    depthImage = hand.depthImage;
    segmentedImage = hand.segmentedImage;
    realCordsMap = new double[depthImage.M*depthImage.N*3];
    if(hand.realCordsMap) {
        for(int j = 0; j < depthImage.N; j++) {
            for(int i = 0; i < depthImage.M; i++) {
                realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N];
                realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N];
                realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N];
            }
        }
    }
    inHandPixels = hand.inHandPixels;
    seed = new int[3];
    if(hand.seed) {
        seed[0] = hand.seed[0];
        seed[1] = hand.seed[1];
        seed[2] = hand.seed[2];
    }
}

Hand& Hand::operator=(Hand& hand) {
    if(!this) {
        return *this;
    }
    if(realCordsMap) {
        delete [] realCordsMap;
        realCordsMap = 0;
    }
    if(seed) {
        delete [] seed;
        seed = 0;
    }
    ID = hand.ID;
    curCMReal[0] = hand.curCMReal[0];
    curCMReal[1] = hand.curCMReal[1];
    curCMReal[2] = hand.curCMReal[2];
    prevCMReal[0] = hand.prevCMReal[0];
    prevCMReal[1] = hand.prevCMReal[1];
    prevCMReal[2] = hand.prevCMReal[2];
    depthImage = hand.depthImage;
    segmentedImage = hand.segmentedImage;
    realCordsMap = new double[depthImage.M*depthImage.N*3];
    if(hand.realCordsMap) {
        for(int j = 0; j < depthImage.N; j++) {
            for(int i = 0; i < depthImage.M; i++) {
                realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N];
                realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N];
                realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N] = hand.realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N];
            }
        }
    }
    inHandPixels = hand.inHandPixels;
    seed = new int[3];
    if(hand.seed) {
        seed[0] = hand.seed[0];
        seed[1] = hand.seed[1];
        seed[2] = hand.seed[2];
    }
    return *this;
}
    
    
    

Hand::Hand(int ID, double* curCMReal, double* prevCMReal, const Image& depthImage) {
    this->ID = ID;
    // add checks that the CM cordinates are valid
    this->curCMReal[0] = curCMReal[0];
    this->curCMReal[1] = curCMReal[1];
    this->curCMReal[2] = curCMReal[2];
    this->prevCMReal[0] = prevCMReal[0];
    this->prevCMReal[1] = prevCMReal[1];
    this->prevCMReal[2] = prevCMReal[2];
    this->depthImage = depthImage;
    inHandPixels = 0;
    realCordsMap = new double[depthImage.M*depthImage.N*3];
    for(int j = 0; j < depthImage.N; j++) {
        for(int i = 0; i < depthImage.M; i++) {
            int tempPic[3];
            tempPic[0] = i;
            tempPic[1] = j;
            tempPic[2] = 1;
            double* tempReal = depthImage.getReal(tempPic);
            realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N] = tempReal[0];
            realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N] = tempReal[1];
            realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N] = tempReal[2];
            delete [] tempReal;
        }
    }
    seed = F2seed();
    segmentedImage = this->F3segmentate();
}


Hand::~Hand() {
    if(this == 0) {
        return;
    }
    if(realCordsMap != 0) {
        delete [] realCordsMap;
        realCordsMap = 0;
    }
    if(seed) {
        delete [] seed;
        seed = 0;
    }
}


double Hand::getDistanceBetween(int picI1, int picJ1, int picI2, int picJ2) const {
    if((picI1 < 0) || (picI1 >= depthImage.M) || (picJ1 < 0) || (picJ1 >= depthImage.N) ||
            (picI2 < 0) || (picI2 >= depthImage.M) || (picJ2 < 0) || (picJ2 >= depthImage.N)) {
        return NAN; // in case the indices are out of bound
    }
    double vector1[3], vector2[3];
    vector1[0] = realCordsMap[picI1 + depthImage.M*picJ1 + 0*depthImage.M*depthImage.N];
    if(isnan(vector1[0])) {
        return NAN; // in case the actual value of the vector on realCordsMap is (NAN,NAN,NAN)
    }
    vector1[1] = realCordsMap[picI1 + depthImage.M*picJ1 + 1*depthImage.M*depthImage.N];
    vector1[2] = realCordsMap[picI1 + depthImage.M*picJ1 + 2*depthImage.M*depthImage.N];
    vector2[0] = realCordsMap[picI2 + depthImage.M*picJ2 + 0*depthImage.M*depthImage.N];
    if(isnan(vector2[0])) {
        return NAN;
    }
    vector2[1] = realCordsMap[picI2 + depthImage.M*picJ2 + 1*depthImage.M*depthImage.N];
    vector2[2] = realCordsMap[picI2 + depthImage.M*picJ2 + 2*depthImage.M*depthImage.N];
    double distanceVector[3];
    distanceVector[0] = vector1[0] - vector2[0];
    distanceVector[1] = vector1[1] - vector2[1];
    distanceVector[2] = vector1[2] - vector2[2];
    return getNorm(distanceVector);
}


// this function allocates memory
double* Hand::F1velocityAprox() const {
    double *realAproximation = new double[3];
    realAproximation[0] = 2*curCMReal[0] -  prevCMReal[0];
    realAproximation[1] = 2*curCMReal[1] -  prevCMReal[1];
    realAproximation[2] = 2*curCMReal[2] -  prevCMReal[2];
    return realAproximation;
}

// this function alocates memory
int* Hand::F2seed() const {
    //double* realCordsAprox = F1velocityAprox();
    double realCordsAprox[3] = {curCMReal[0], curCMReal[1], curCMReal[2]};
    double tempRealVector[3];
    double tempDistance;
    double minDistance = SEEDINFNORM;
    int minI, minJ;
    for(int j = 0; j < depthImage.N; j += parture()) {
        for(int i = 0; i < depthImage.M; i += parture()) {
            tempRealVector[0] = realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N];
            if(isnan(tempRealVector[0])) {
                continue;
            }
            int count = 0;
            if((i > 0) && (!isnan(depthImage.image[i-1 + j*depthImage.M]))) { count++; }
            if((i < depthImage.M - 1) && (!isnan(depthImage.image[i+1 + j*depthImage.M]))) { count++; }
            if((j > 0) && (!isnan(depthImage.image[i + (j-1)*depthImage.M]))) { count++; }
            if((j < depthImage.N - 1) && (!isnan(depthImage.image[i + (j+1)*depthImage.M]))) { count++; }
            if(count < 3) { continue; }
            tempRealVector[1] = realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N];
            tempRealVector[2] = realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N];
            if(fabs(tempRealVector[2] - curCMReal[2]) > 0.12) {     // add const
                continue;
            }
            tempRealVector[0] = tempRealVector[0] - realCordsAprox[0];
            tempRealVector[1] = tempRealVector[1] - realCordsAprox[1];
            tempRealVector[2] = tempRealVector[2] - realCordsAprox[2];
            tempDistance = getNorm(tempRealVector);
            if(tempDistance < minDistance) {
                minDistance = tempDistance;
                minI = i;
                minJ = j;
            }
        }
    }
    //delete [] realCordsAprox;
    int* seed = new int[3];
    if(minDistance == SEEDINFNORM) {
        seed[0] = 0;                    // throw error instead?
        seed[1] = 0;
        seed[2] = 0;
        return seed;
    }
    seed[0] = minI;
    seed[1] = minJ;
    seed[2] = 1;
    return seed;
}







Image Hand::F3segmentate() {
    if(seed[2] == 0) {
        return depthImage;
    }
    double dist;
    double* result = new double[depthImage.M*depthImage.N];
    for(int j = 0; j < depthImage.N; j++) {
        for(int i = 0; i < depthImage.M; i++) {
            result[i + depthImage.M*j] = OUTERPOINT;
        }
    }
    Queue queue = Queue(depthImage.M, depthImage.N, seed);
    while(queue.getNext()) {
        int curI = queue.curI;
        int curJ = queue.curJ;
        if(queue.value > MAXDISTANCE) {
            break;
        }
        result[curI + depthImage.M*curJ] = INNERPOINT;
        inHandPixels++;
        if(curI > 0) {
            dist = getDistanceBetween(curI, curJ, curI-1, curJ);
            if(!isnan(dist) && dist < JUMPTHRESH) {
            queue.addToQueue(curI - 1, curJ, getDistanceBetween(curI - 1, curJ, seed[0], seed[1]));
            }
        }
        if(curI < depthImage.M - 1) {
            dist = getDistanceBetween(curI, curJ, curI+1, curJ);
            if(!isnan(dist) && dist < JUMPTHRESH) {
            queue.addToQueue(curI + 1, curJ, getDistanceBetween(curI + 1, curJ, seed[0], seed[1]));
            }
        }
        if(curJ > 0) {
            dist = getDistanceBetween(curI, curJ, curI, curJ-1);
            if(!isnan(dist) && dist < JUMPTHRESH) {
            queue.addToQueue(curI, curJ - 1, getDistanceBetween(curI, curJ - 1, seed[0], seed[1]));
            }
        }
        if(curJ < depthImage.N - 1) {
            dist = getDistanceBetween(curI, curJ, curI, curJ+1);
            if(!isnan(dist) && dist < JUMPTHRESH) {
            queue.addToQueue(curI, curJ + 1, getDistanceBetween(curI, curJ + 1, seed[0], seed[1]));
            }
        }
    }
    Image segmented = Image(result, depthImage.M, depthImage.N);
    delete [] result;
    return segmented;
}

static int* getRandomValues(int n, int range) {
    if(n > range) {
        return 0;
    }
    srand((unsigned int)time(0));
    int* random = new int[n];
    int *chosen = new int[range];
    for(int i = 0; i < range; i++) {
        chosen[i] = 0;
    }
    for(int i = 0; i < n; i++) {
        int tempRand = rand() % (range - i) + 1;
        int j = 0;
        while(tempRand) {
            if(!chosen[j]) {
                tempRand--;
            }
            j++;
        }
        chosen[j-1] = 1;
        random[i] = j-1;
    }
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-1-i; j++) {
            if(random[j] > random[j+1]) {
                int temp = random[j];
                random[j] = random[j+1];
                random[j+1] = temp;
            }
        }
    }
    return random;
}


int* Hand::F4boundary() const {
    if((seed[2] == 0) || (!inHandPixels)) {
        int* error = new int[3];
        error[2] = 0;
        return error;
    }
    int* random = getRandomValues(20, inHandPixels);
    if(!random) {
        int* error = new int[3];
        error[2] = 0;
        return error;
    }
    int randomPoints[20][2];
    int pixelCounter = -1;
    int pointCounter = 0;
    for(int j = 0; j < segmentedImage.N; j++) {
        for(int i = 0; i < segmentedImage.M; i++) {
            if(segmentedImage.image[i + segmentedImage.M*j] == INNERPOINT) {
                pixelCounter++;
            }
            if(random[pointCounter] == pixelCounter) {
                randomPoints[pointCounter][0] = i;
                randomPoints[pointCounter][1] = j;
                pointCounter++;
            }
            if(pointCounter >= 20) {
                break;
            }
        }
    }
    delete [] random;        
    int scores[20] = {0};
    Image circ = depthImage.getCircumference();
    for(int j = 0; j < segmentedImage.N; j += parture()) {
        for(int i = 0; i < segmentedImage.M; i+= parture()) {
            if(segmentedImage.image[i + segmentedImage.M*j] != INNERPOINT) {
                continue;
            }
            /*if(!(((i > 0) && (segmentedImage.image[(i-1) + segmentedImage.M*j] == OUTERPOINT)) ||
               ((i < segmentedImage.M - 1) && (segmentedImage.image[(i+1) + segmentedImage.M*j] == OUTERPOINT)) ||
               ((j > 0) && (segmentedImage.image[i + segmentedImage.M*(j-1)] == OUTERPOINT)) ||
               ((j < segmentedImage.N - 1) && (segmentedImage.image[i + segmentedImage.M*(j+1)] == OUTERPOINT)))) {
                continue;
            }*/
            if(circ.image[i + circ.M*j] != 0) {
                continue;
            }
            for(int k = 0; k < 20; k++) {
                double distance = getDistanceBetween(i, j, randomPoints[k][0], randomPoints[k][1]);
                if(distance < MAXBNDRYDIST) {
                    scores[k]++;
                }
            }
        }
    }
    int maxIndex = 0, maxScore = scores[0];
    for(int i = 1; i < 20; i++) {
        if(scores[i] > maxScore) {
            maxIndex = i;
            maxScore = scores[i];
        }
    }
    int* bPoint = new int[3];
    bPoint[0] = randomPoints[maxIndex][0];
    bPoint[1] = randomPoints[maxIndex][1];
    bPoint[2] = 1;
    return bPoint;
}


double* Hand::F5mean() const {
    int* boundaryPoint = this->F4boundary();
    if(boundaryPoint[2] == 0) {
        delete boundaryPoint;
        double* error = new double[3];
        error[0] = error[1] = error[2] = 0;
        return error;
    }
    int count = 0;
    double* average = new double[3];
    average[0] = 0;
    average[1] = 0;
    average[2] = 0;
    for(int j = 0; j < depthImage.N; j+=parture()) {
        for(int i = 0; i < depthImage.M; i+=parture()) {
            double distance = getDistanceBetween(boundaryPoint[0], boundaryPoint[1], i, j);
            if((isnan(distance)) || (distance > MEANTHRESH)) {
                continue;
            }
            if(segmentedImage.image[i + segmentedImage.M*j] == INNERPOINT) {
                count++;
                average[0] += realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N];
                average[1] += realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N];
                average[2] += realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N];
            }
        }
    }
    delete [] boundaryPoint;
    if(count > 0) {
        average[0] /= count;
        average[1] /= count;
        average[2] /= count;
        return average;
    }
    average[0] = 0;
    average[1] = 0;
    average[2] = 0;
    return average;
}




int* Hand::F6nextCenterOfMass() const {
    double* estimatedMean = this->F5mean(); // WTF?
    if((estimatedMean[0] == 0) && (estimatedMean[1] == 0) && (estimatedMean[2] == 0)) {
        delete estimatedMean;
        int* error = new int[3];
        error[0] = error[1] = error[2] = 0;
        return error;
    }
    //mexPrintf("\nestmean[0] = %lf\nestmean[1] = %lf\nestmean[2] = %lf\n", estimatedMean[0], estimatedMean[1], estimatedMean[2]);
    int* nextCM = new int[3];
    nextCM[0] = 0;
    nextCM[1] = 0;
    nextCM[2] = 1;
    double maxCosinus = -1;
    for(int j = 0; j < depthImage.N; j+=parture()) {
        for(int i = 0; i < depthImage.M; i+=parture()) {
            if(segmentedImage.image[i + j*depthImage.M] != INNERPOINT) {
                continue;
            }
            double tempVector[3];
            tempVector[0] = realCordsMap[i + j*depthImage.M + 0*depthImage.M*depthImage.N];
            tempVector[1] = realCordsMap[i + j*depthImage.M + 1*depthImage.M*depthImage.N];
            tempVector[2] = realCordsMap[i + j*depthImage.M + 2*depthImage.M*depthImage.N];
            // should I check that it's not NAN? I think that if segmented[i,j] == innerpoint then map[i,j,x] != NAN
            double tempCosinus = estimatedMean[0]*tempVector[0] + estimatedMean[1]*tempVector[1] + estimatedMean[2]*tempVector[2];
            tempCosinus = tempCosinus/(getNorm(tempVector) * getNorm(estimatedMean));
            if(tempCosinus > maxCosinus) {
                maxCosinus = tempCosinus;
                nextCM[0] = i;
                nextCM[1] = j;
            }
        }
    }
    delete [] estimatedMean;
    return nextCM;
}