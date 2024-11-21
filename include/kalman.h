#ifndef KALMAN_H
#define KALMAN_H

//same k1 k2 gain for roll and pitch 
#define K1 0.618
#define K2 0.00000386269496

float update(float x, float z1, float z2, float k1, float k2);
float predict(float x);

#endif