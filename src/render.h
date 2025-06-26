#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED


#include <vector>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <thread>

#include "global.h"
#include "data_structures.h"
#include "funcstore.h"

using namespace std;

//intersection plane and straight
//N is normalvector of plane and PL_P is point of plane
//G is vector of stright and ST_P is point of straight
inline void inter_pl_str(const float Nvec[3], const float Npoint[3], const float Gvec[3],float schnitt[3]){
//doubles as c and as r fom your formula
float c = (Nvec[0]*Npoint[0]+Nvec[1]*Npoint[1]+Nvec[2]*Npoint[2])/(Gvec[0]*Nvec[0]+Gvec[1]*Nvec[1]+Gvec[2]*Nvec[2]);

schnitt[0]=c*Gvec[0];
schnitt[1]=c*Gvec[1];
schnitt[2]=c*Gvec[2];
}

//crossproduct
inline void cross_prod(const float aX,const float aY,const float aZ,const float bX,const float bY,const float bZ,float Nvec[3]){
Nvec[0]=(aY*bZ-aZ*bY);
Nvec[1]=(aZ*bX-aX*bZ);
Nvec[2]=(aX*bY-aY*bX);
}

//finds out if a point is in a givent triangle
inline bool in_trig_barricentric(const float px,const float py,const float trig[6],float& w1, float& w2){

float CYmAY = trig[5]-trig[1];

w1=(trig[0]*(CYmAY)+(py-trig[1])*(trig[4]-trig[0])-px*(CYmAY))/((trig[3]-trig[1])*(trig[4]-trig[0])-(trig[2]-trig[0])*(CYmAY));
w2=(py-trig[1]-w1*(trig[3]-trig[1]))/(CYmAY);


return w1>=0&&w2>=0&&(w1+w2)<=1;
}

//finds the texture of a point relative to a given triange and uv
//affine texture issue
inline void find_texture_rgb(const int* texture,const float uv[6],const float w1, const float w2,const int dim_x,const int dim_y,int colour[3]){
    float u = uv[0] + w1 * (uv[2] - uv[0]) + w2 * (uv[4] - uv[0]);
    float v = 1.0f - (uv[1] + w1*(uv[3]-uv[1]) + w2*(uv[5]-uv[1]));
    int px = (int)(u * (dim_x-1)) & (dim_x-1);
    int py = (int)(v * (dim_y-1)) & (dim_y-1);
    int location = (py * dim_x + px)*3;

    colour[0]=texture[location];
    colour[1]=texture[location+1];
    colour[2]=texture[location+2];
}

//culls all poligons behind me
inline bool cull(const camera_3d cam,const float* verts){
return (verts[1] > cam.data[8]) && (verts[4] > cam.data[8]) && (verts[7] > cam.data[8]);
}

//render triangles
inline void render_tris(const int tris_amount, const int start,int which,float* depth_buffer,int* colour_buffer) {
    int scr_size = cam.data[6]*cam.data[7];
    float aspect = cam.data[6]/cam.data[7]/2;
    float tris[9];
    int colour[3];
    float w1,w2;
    float Gvec[3];
    float intersect[3];
    for (int i = 0; i < tris_amount; i++) {
        float* uv=&objects[which].uv[i*6+start*6/9];
        memcpy(&tris[0],&objects[which].vert[start+i*9],sizeof(float)*9);

        rot_3d(tris[0],tris[1],tris[2], objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);
        rot_3d(tris[3],tris[4],tris[5], objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);
        rot_3d(tris[6],tris[7],tris[8], objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);

        tris[0]  +=  objects[which].trans[6]-cam.data[3];
        tris[1]  +=  objects[which].trans[7]-cam.data[4];
        tris[2]  +=  objects[which].trans[8]-cam.data[5];

        tris[3]  +=  objects[which].trans[6]-cam.data[3];
        tris[4]  +=  objects[which].trans[7]-cam.data[4];
        tris[5]  +=  objects[which].trans[8]-cam.data[5];

        tris[6]  +=  objects[which].trans[6]-cam.data[3];
        tris[7]  +=  objects[which].trans[7]-cam.data[4];
        tris[8]  +=  objects[which].trans[8]-cam.data[5];

        rot_3d(tris[0],tris[1],tris[2], cam.data[0],cam.data[1],cam.data[2]);
        rot_3d(tris[3],tris[4],tris[5], cam.data[0],cam.data[1],cam.data[2]);
        rot_3d(tris[6],tris[7],tris[8], cam.data[0],cam.data[1],cam.data[2]);

        if (cull(cam,tris)) {

        tris[2] *= aspect;
        tris[5] *= aspect;
        tris[8] *= aspect;

        //per triangle depth calc
        float Nvec[3];
        cross_prod(tris[0]-tris[6],tris[1]-tris[7],tris[2]-tris[8],
                   tris[0]-tris[3],tris[1]-tris[4],tris[2]-tris[5],Nvec);

        float Npoint[3]= {tris[0], tris[1], tris[2]};

        tris[0]=tris[0] /tris[1]*cam.data[8];
        tris[1]=tris[2] /tris[1]*cam.data[8];
        tris[2]=tris[3] /tris[4]*cam.data[8];
        tris[3]=tris[5] /tris[4]*cam.data[8];
        tris[4]=tris[6] /tris[7]*cam.data[8];
        tris[5]=tris[8] /tris[7]*cam.data[8];

        // Convert bounding box to pixel coordinates and clamp to screen bounds
        int bound_min_X = boost::algorithm::clamp((int)((std::min({tris[4], tris[0], tris[2]}) + 1) * 0.5 * cam.data[6]), 0, (int)cam.data[6]);
        int bound_max_X = boost::algorithm::clamp((int)((std::max({tris[4], tris[0], tris[2]}) + 1) * 0.5 * cam.data[6]), 0, (int)cam.data[6]-1);
        int bound_min_Y = boost::algorithm::clamp((int)((std::min({tris[5], tris[1], tris[3]}) + 1) * 0.5 * cam.data[7]), 0, (int)cam.data[7]);
        int bound_max_Y = boost::algorithm::clamp((int)((std::max({tris[5], tris[1], tris[3]}) + 1) * 0.5 * cam.data[7]), 0, (int)cam.data[7]);

        // Iterate over pixel coordinates
        for (int y = bound_min_Y; y <= bound_max_Y; y++) {
            float ny = (y / (float)cam.data[7]) * 2 - 1;
            for (int x = bound_min_X; x <= bound_max_X; x++) {
                // Convert pixel to normalized coordinates
                float nx = (x / (float)cam.data[6]) * 2 - 1;

                // Check if pixel is inside the triangle
                if (in_trig_barricentric(nx, ny, tris,w1,w2)) {

                    int pixel_index = x + y * cam.data[6];
                    Gvec[0]=nx;Gvec[1]=cam.data[8];Gvec[2]=ny;
                    inter_pl_str(Nvec,Npoint,Gvec,intersect);
                    float temp=sqrt(intersect[0]*intersect[0]+intersect[1]*intersect[1]+intersect[2]*intersect[2]);

                    if (pixel_index >= 0 && pixel_index < scr_size&&temp<depth_buffer[pixel_index]) {
                        depth_buffer[pixel_index]=temp;

                        find_texture_rgb(&textures[objects[which].text_p].texture[0],&objects[which].uv[i*6+start*6/9],w1,w2,
                                          textures[objects[which].text_p].dim_x,textures[objects[which].text_p].dim_y,colour);

                        memcpy(&colour_buffer[pixel_index*3],colour,3*sizeof(int));
                    }
                }
            }
        }
    }
}
}

//hyperthread the rendering
inline void render_tris_acell(int o) {

        int tris_per_thread = objects[o].vert.size() / 9 / thread_numb;
        int overflow_tris = objects[o].vert.size() / 9 % thread_numb;

        vector<thread> thread_pool;

        if (winsize_change){
            for (int i = 0; i < thread_numb; i++) {
                depth_buffs[i].resize(cam.data[6] * cam.data[7]);
                colour_buffs[i].resize(cam.data[6] * cam.data[7]*3);
            }
            winsize_change=false;
        }

        for (int i = 0; i < thread_numb; i++) {

            std::fill(depth_buffs[i].begin(), depth_buffs[i].end(), INFINITY);
            std::fill(colour_buffs[i].begin(), colour_buffs[i].end(), 216);

            thread_pool.emplace_back(render_tris, tris_per_thread, tris_per_thread * 9 * i,o,&depth_buffs[i][0],&colour_buffs[i][0]);
        }
        render_tris(overflow_tris,tris_per_thread*thread_numb*9,o,depth_buffer,colour_buffer);
        // Wait for completion
        for (int i=0;i<thread_numb;i++){
            thread_pool[i].join();
            for (int x=0;x<cam.data[6]*cam.data[7];x++){
                if(depth_buffs[i][x]<depth_buffer[x]){
                    depth_buffer[x]=depth_buffs[i][x];
                    memcpy(&colour_buffer[x*3],&colour_buffs[i][x*3],3*sizeof(int));
                }
            }
        }
}

#endif // RENDER_H_INCLUDED
