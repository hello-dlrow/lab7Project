#include "kalman.h"
//x:  prior prediction of attitude
//z1: meas from acc
//z2: meas from gyro
//k1: convergence kalman gain for acc
//k2: convergence kaiman gain for gyro
float update(float x, float z1, float z2, float k1, float k2){
    float y1 = z1 - x;    //acc's residual
    float y2 = z2 - x;    //gyro's residual

    x = x + k1 * y1 + k2 * y2;
    return x;
}

//state transform function

float predict(float x){
    return x;
}