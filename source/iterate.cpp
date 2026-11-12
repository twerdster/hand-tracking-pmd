#include "mex.h"
#include "Image.h"
#include "Hand.h"
#include <string.h>
#include <time.h>

#define MAXHANDS 2
#define LOWACCURACY 0.05
#define BALL_MAX_RADIUS 0.5
#define BALL_MIN_RADIUS 0.15
#define INNERPOINT 1

using namespace HT;

static bool isnan(double var) {
    volatile double d = var;
    return d != d;
}





                                                    // CODE CAN BE OPTIMIZED BY USING A SHARED REALCORDS ARRAY
int* recognize(Image& depthImage) {                 // PERHAPS MAKING IT STATIC
    int minI, minJ, i, j;
    minI = minJ = 0;
    for(j = 1; j < depthImage.N-1; j+=3) {
        for(i = 1; i < depthImage.M-1; i+=3) {
            if(!isnan(depthImage.image[i + j*depthImage.M])) {
//                 int tempPicCords[3];
//                 tempPicCords[0] = i;
//                 tempPicCords[1] = j;
//                 tempPicCords[2] = 1;
//                 double* tempV = depthImage.getReal(tempPicCords);
//                 double tempLength = sqrt(tempV[0]*tempV[0] + tempV[1]*tempV[1] + tempV[2]*tempV[2]);
                double tempLength = depthImage.image[i + j*depthImage.M];
                if((tempLength < BALL_MAX_RADIUS) && (tempLength > BALL_MIN_RADIUS) &&
                   (!isnan(depthImage.image[i+1 + j*depthImage.M])) && 
                   (!isnan(depthImage.image[i-1 + j*depthImage.M]))){      // deffinetley stop on edges
                    minI = i;
                    minJ = j;
                    break;
                }
                //delete [] tempV;
            }
        }
    }
    int* result = new int[3];
    result[0] = minI;
    result[1] = minJ;
    result[2] = 1;
    if((i == depthImage.M) && (j == depthImage.N)) {
        result[2] = 0;
    }
    return result;
}



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    int* support = (int*)mxGetPr(prhs[0]); //get support array
    int* nextSupport = new int[MAXHANDS];       // add an alocation check
    for(int i = 0; i < MAXHANDS; i++) {
        nextSupport[i] = support[i];
    }    
    Image depthImage = Image((double*)mxGetPr(prhs[1]), int(mxGetScalar(prhs[2])), int(mxGetScalar(prhs[3])));  // pic / m / n
    double* curCMReal = (double*)(mxGetPr(prhs[4])); //get matrix?
    double* prevCMReal = (double*)(mxGetPr(prhs[5])); // get matrix?
    double* nextCMReal = new double[MAXHANDS*3];
    int* nextCM = new int[MAXHANDS*3];
    Hand hand[MAXHANDS];
    int flag = 0;
    for(int i = 0; i < MAXHANDS; i++) {
        if(nextSupport[i]) {
            double curCMRealTMP[3] = { curCMReal[3*i], curCMReal[3*i+1], curCMReal[3*i+2] };
            if(nextSupport[i] < 0) {
                hand[i] = Hand(i, curCMRealTMP, curCMRealTMP, depthImage);
                nextSupport[i] *= -1;
            } else {
                double prevCMRealTMP[3] = { prevCMReal[3*i], prevCMReal[3*i+1], prevCMReal[3*i+2] };
                hand[i] = Hand(i, prevCMRealTMP, prevCMRealTMP, depthImage);
            }
            int* tempNextCM = hand[i].F6nextCenterOfMass();
            if(tempNextCM[2] == 0)
            {
                delete tempNextCM;
                nextSupport[i] = 0;
                flag = 1;
                continue;
            }
            nextCM[3*i]   = tempNextCM[0];
            nextCM[3*i+1] = tempNextCM[1];
            nextCM[3*i+2] = tempNextCM[2];
            nextCMReal[3*i]   = hand[i].realCordsMap[tempNextCM[0] + tempNextCM[1]*depthImage.M + 0*depthImage.M*depthImage.N];
            nextCMReal[3*i+1] = hand[i].realCordsMap[tempNextCM[0] + tempNextCM[1]*depthImage.M + 1*depthImage.M*depthImage.N];
            nextCMReal[3*i+2] = hand[i].realCordsMap[tempNextCM[0] + tempNextCM[1]*depthImage.M + 2*depthImage.M*depthImage.N];
            delete tempNextCM;
        } else {
            flag = 1;
        }
    }
    if(flag) {
        int* newSeed = recognize(depthImage);
        int emptySpot;
        for(emptySpot = 0; emptySpot < MAXHANDS; emptySpot++) {
            if(!nextSupport[emptySpot]) {
                break;
            }
        }
        if(newSeed[2] != 0) {                   // if a new seed was found
            int i;
            for(i = 0; i < MAXHANDS; i++) {
                if((nextSupport[i] > 0) && 
                   (hand[i].segmentedImage.image[newSeed[0] 
                    + hand[i].segmentedImage.M*newSeed[1]] == INNERPOINT)) {
                    break;
                }
            }
            if(i == MAXHANDS) {
                nextSupport[emptySpot] = -1;
                double* tempNextCM = depthImage.getReal(newSeed); // verify syntax
                nextCMReal[3*emptySpot]   = tempNextCM[0];
                nextCMReal[3*emptySpot+1] = tempNextCM[1];
                nextCMReal[3*emptySpot+2] = tempNextCM[2];
                nextCM[3*emptySpot] = newSeed[0];
                nextCM[3*emptySpot+1] = newSeed[1];
                nextCM[3*emptySpot+2] = newSeed[2];
                delete [] tempNextCM;
            }
        } else {        // can take this line out in case I show point iff support[i] = 1
            nextCM[3*emptySpot+2] = 0;
        }
        delete [] newSeed;
    }
 
/*    
    Hand temp = Hand(1, curCMReal, prevCMReal, depthImage);
    nextCM = temp.F6nextCenterOfMass();
    nextCMReal[0]   = temp.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 0*depthImage.M*depthImage.N];
    nextCMReal[1] = temp.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 1*depthImage.M*depthImage.N];
    nextCMReal[2] = temp.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 2*depthImage.M*depthImage.N];
    
    
    mwSize dims[2];
    dims[0] = 1;
    dims[1] = MAXHANDS;
    plhs[0] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[0]), support, sizeof(int)*dims[0]*dims[1]);
    
    dims[0] = MAXHANDS;
    dims[1] = 3;
    plhs[0] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[1]), nextCM, sizeof(int)*dims[0]*dims[1]);
    delete [] nextCM;
    
    plhs[1] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[2]), nextCMReal, sizeof(double)*dims[0]*dims[1]);
    delete [] nextCMReal;

    
    Hand test = Hand(1,curCMReal, prevCMReal, depthImage);
    
    int* nextCMResult = test.F6nextCenterOfMass();
    nextCM[0] = nextCMResult[0];
    nextCM[1] = nextCMResult[1];
    nextCM[2] = nextCMResult[2];
    delete nextCMResult;
    nextCMReal[0] = test.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 0*depthImage.M*depthImage.N];
    nextCMReal[1] = test.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 1*depthImage.M*depthImage.N];
    nextCMReal[2] = test.realCordsMap[nextCM[0] + nextCM[1]*depthImage.M + 2*depthImage.M*depthImage.N];
    
*/    

    mwSize dims[2];
 
    
    dims[0] = 1;
    dims[1] = MAXHANDS;
    plhs[0] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[0]), nextSupport, sizeof(int)*dims[0]*dims[1]);
    delete [] nextSupport;
    
    dims[0] = 1;
    dims[1] = 3*MAXHANDS;
    plhs[1] = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[1]), nextCM, sizeof(int)*3*MAXHANDS);
    delete [] nextCM;
    
    plhs[2] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
    memcpy(mxGetPr(plhs[2]), nextCMReal, sizeof(double)*3*MAXHANDS);
    delete [] nextCMReal;
    
    
    
}
    
            