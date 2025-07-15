#ifndef KONMATH_H_INCLUDED
#define KONMATH_H_INCLUDED

#include "global.h"

// 2D Rotation
inline void rotate_2d(float& X, float& Y, const float angle) {
    float coa = cos(angle);float sia=sin(angle);
    float temp = X * coa- Y * sia;
    Y = X * sia + Y * coa;
    X = temp;
}
// 3D Rotiation
inline void rotate_3d(float& X, float& Y, float& Z, const float ax, const float ay, const float az) {

    float coa = cos(az);float sia=sin(az);
    float temp = X * coa- Y * sia;
    Y = X * sia + Y * coa;
    X = temp;

    coa = cos(ay); sia=sin(ay);
    X = temp * coa- Z * sia;
    Z = temp * sia + Z * coa;

    coa = cos(ax); sia=sin(ax);
    temp = Y * coa-Z * sia;
    Z = Y * sia + Z * coa;
    Y = temp;
}

//crossproduct
//crossproduct
inline void crossproduct(const float aX,const float aY,const float aZ,
                         const float bX,const float bY,const float bZ,
                         float Nvec[3]){

Nvec[0]=(aY*bZ-aZ*bY);
Nvec[1]=(aZ*bX-aX*bZ);
Nvec[2]=(aX*bY-aY*bX);
}

#endif // KONMATH_H_INCLUDED
