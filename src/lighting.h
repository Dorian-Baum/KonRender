#ifndef LIGHTING_H_INCLUDED
#define LIGHTING_H_INCLUDED

#include "KonMath.h"

//CURRENTLY WORKING ON IMPLEMENTING

inline float triangle_area_3D(float ax, float ay, float az,
                              float bx, float by, float bz,
                              float cx, float cy, float cz){

float abx=bx-ax;float aby=by-ay;float abz=bz-az;
float acx=cx-ax;float acy=cy-ay;float acz=cz-az;

return 0.5*sqrtf(pow(aby*acz-abz*acy,2)+pow(abx*acz-abz*acx,2)+pow(abx*acy-aby*acx,2));
}

inline bool is_inside_triangle_3d(float ax, float ay, float az,
                                  float bx, float by, float bz,
                                  float cx, float cy, float cz,
                                  float px, float py, float pz){

float trig_3 = triangle_area_3D(ax,ay,az,bx,by,bz,px,py,pz)+triangle_area_3D(ax,ay,az,px,py,pz,cx,cy,cz)+triangle_area_3D(px,py,pz,bx,by,bz,cx,cy,cz);

return triangle_area_3D(ax,ay,az,bx,by,bz,cx,cy,cz)-trig_3 < 1e-6f;
}

inline bool is_inside_triangle_barycentric_3d(float ax, float ay, float az,
                                              float bx, float by, float bz,
                                              float cx, float cy, float cz,
                                              float px, float py, float pz){
    // Compute vectors
    float v0x = cx - ax, v0y = cy - ay, v0z = cz - az;
    float v1x = bx - ax, v1y = by - ay, v1z = bz - az;
    float v2x = px - ax, v2y = py - ay, v2z = pz - az;

    // Compute dot products
    float dot00 = v0x * v0x + v0y * v0y + v0z * v0z;
    float dot01 = v0x * v1x + v0y * v1y + v0z * v1z;
    float dot02 = v0x * v2x + v0y * v2y + v0z * v2z;
    float dot11 = v1x * v1x + v1y * v1y + v1z * v1z;
    float dot12 = v1x * v2x + v1y * v2y + v1z * v2z;

    // Compute barycentric coordinates
    float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
    float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

inline void intersection_plane_straight(const float Nvec[3], float Npoint[3],
                                        const float Gvec[3],const float Gpoint[3],
                                        float schnitt[3]){

Npoint[0]-=Gpoint[0];
Npoint[2]-=Gpoint[1];
Npoint[2]-=Gpoint[2];

float c = (Nvec[0]*Npoint[0]+Nvec[1]*Npoint[1]+Nvec[2]*Npoint[2])/(Gvec[0]*Nvec[0]+Gvec[1]*Nvec[1]+Gvec[2]*Nvec[2]);

schnitt[0]=c*Gvec[0]+Gpoint[0];
schnitt[1]=c*Gvec[1]+Gpoint[1];
schnitt[2]=c*Gvec[2]+Gpoint[2];
}

#endif // LIGHTING_H_INCLUDED
