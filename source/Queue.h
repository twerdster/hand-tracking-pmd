#include "mex.h"
#ifndef QUEUE_H_
#define QUEUE_H_



namespace HT {
static bool isnan(double var) {
    volatile double d = var;
    return d != d;
}
    
class Queue {           // there is an assumption that the seed will always be inside the map
public:                         // this case needs to be handled
    int M, N;
    double* queue;
    int curI;
    int curJ;
    double value;
    int flag;
    Queue(int M, int N, int* seed) { 
        this->M = M;
        this->N = N;
        queue = new double[M*N];
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < M; i++) {
                queue[i + M*j] = 100;
            }
        }
        queue[seed[0] + M*seed[1]] = 0;
    }
    ~Queue() {
        if(queue) {
            delete [] queue;
        }
    }
    bool getNext() {
        double minValue = 100;
        int minI, minJ;
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < M; i++) {               // perhaps remove the != inf
                if((queue[i + M*j]  < 100) && (queue[i + M*j] > -100) && (queue[i + M*j] < minValue)) {
                    minValue = queue[i + M*j];
                    minI = i;
                    minJ = j;
                }
            }
        }
        if(minValue != 100) {
            curI = minI;
            curJ = minJ;
            value = queue[minI + M*minJ];
            queue[minI + M*minJ] = -100;     //here I mark that (minI,minJ) has been used
            return true;
        }
        return false;           // when the queue is empty;
    }
    void addToQueue(int addI, int addJ, double distance) {
        if(isnan(distance)) { //index out of bound on depthImage or map or the values in it are NAN
            return;
        }
        if(queue[addI + M*addJ] == 100) {
            queue[addI + M*addJ] = distance;
        } 
    }
};

}
#endif