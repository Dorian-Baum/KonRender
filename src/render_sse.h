#ifndef RENDER_SSE_H_INCLUDED
#define RENDER_SSE_H_INCLUDED

#include "render.h"
#include <pmmintrin.h>

using namespace std;


//converts sse data to normal data
inline void convert_mm(float tris[36], __m128 ax,__m128 ay,__m128 az, __m128 bx,__m128 by,__m128 bz, __m128 cx,__m128 cy,__m128 cz,
                             __m128 Nvec_x,__m128 Nvec_y,__m128 Nvec_z,__m128 Npoint_x,__m128 Npoint_y,__m128 Npoint_z,float Nvec[4][3],float Npoint[4][3]){
float* c = static_cast<float*>(_mm_malloc(4 * sizeof(float), 16));

        _mm_store_ps(c, ax);
        tris[0] = c[0];tris[9] = c[1];tris[18] = c[2];tris[27] = c[3];
        _mm_store_ps(c, ay);
        tris[1] = c[0];tris[10] = c[1];tris[19] = c[2];tris[28] = c[3];
        _mm_store_ps(c, az);
        tris[2] = c[0];tris[11] = c[1];tris[20] = c[2];tris[29] = c[3];

        _mm_store_ps(c, bx);
        tris[3] = c[0];tris[12] = c[1];tris[21] = c[2];tris[30] = c[3];
        _mm_store_ps(c, by);
        tris[4] = c[0];tris[13] = c[1];tris[22] = c[2];tris[31] = c[3];
        _mm_store_ps(c, bz);
        tris[5] = c[0];tris[14] = c[1];tris[23] = c[2];tris[32] = c[3];

        _mm_store_ps(c, cx);
        tris[6] = c[0];tris[15] = c[1];tris[24] = c[2];tris[33] = c[3];
        _mm_store_ps(c, cy);
        tris[7] = c[0];tris[16] = c[1];tris[25] = c[2];tris[34] = c[3];
        _mm_store_ps(c, cz);
        tris[8] = c[0];tris[17] = c[1];tris[26] = c[2];tris[35] = c[3];

        _mm_store_ps(c,Nvec_x);
        Nvec[0][0]=c[0];Nvec[1][0]=c[1];Nvec[2][0]=c[2];Nvec[3][0]=c[3];
        _mm_store_ps(c,Nvec_y);
        Nvec[0][1]=c[0];Nvec[1][1]=c[1];Nvec[2][1]=c[2];Nvec[3][1]=c[3];
        _mm_store_ps(c,Nvec_z);
        Nvec[0][2]=c[0];Nvec[1][2]=c[1];Nvec[2][2]=c[2];Nvec[3][2]=c[3];

        _mm_store_ps(c,Npoint_x);
        Npoint[0][0]=c[0];Npoint[1][0]=c[1];Npoint[2][0]=c[2];Npoint[3][0]=c[3];
        _mm_store_ps(c,Npoint_y);
        Npoint[0][1]=c[0];Npoint[1][1]=c[1];Npoint[2][1]=c[2];Npoint[3][1]=c[3];
        _mm_store_ps(c,Npoint_z);
        Npoint[0][2]=c[0];Npoint[1][2]=c[1];Npoint[2][2]=c[2];Npoint[3][2]=c[3];

        _mm_free(c);
}
//sse crosspoduct
inline void cross_prod(__m128 ax,__m128 ay,__m128 az,__m128 bx,__m128 by,__m128 bz,__m128& Nvec_x,__m128& Nvec_y,__m128& Nvec_z){
Nvec_x = _mm_sub_ps(_mm_mul_ps(ay,bz),_mm_mul_ps(az,by));
Nvec_y = _mm_sub_ps(_mm_mul_ps(az,bx),_mm_mul_ps(ax,bz));
Nvec_z = _mm_sub_ps(_mm_mul_ps(ax,by),_mm_mul_ps(ay,bx));
}
//sse 3d rotation
inline void rot_3d(__m128& X, __m128& Y, __m128& Z, const float ax, const float ay, const float az) {

    float coa = cos(az);float sia=sin(az);
    __m128 co4 = _mm_setr_ps(coa,coa,coa,coa);
    __m128 si4 = _mm_setr_ps(sia,sia,sia,sia);

    __m128 temp = _mm_sub_ps(_mm_mul_ps(X,co4),_mm_mul_ps(Y,si4));
    Y = _mm_add_ps(_mm_mul_ps(X,si4),_mm_mul_ps(Y,co4));
    X = temp;

    coa = cos(ay); sia=sin(ay);
    co4 = _mm_setr_ps(coa,coa,coa,coa);
    si4 = _mm_setr_ps(sia,sia,sia,sia);

    X = _mm_sub_ps(_mm_mul_ps(temp,co4),_mm_mul_ps(Z,si4));
    Z = _mm_add_ps(_mm_mul_ps(temp,si4),_mm_mul_ps(Z,co4));

    coa = cos(ax); sia=sin(ax);
    co4 = _mm_setr_ps(coa,coa,coa,coa);
    si4 = _mm_setr_ps(sia,sia,sia,sia);

    temp = _mm_sub_ps(_mm_mul_ps(Y,co4),_mm_mul_ps(Z,si4));
    Z = _mm_add_ps(_mm_mul_ps(Y,si4),_mm_mul_ps(Z,co4));
    Y = temp;
}

//sse culling
__m128 cull(__m128 fov, __m128 ay, __m128 by, __m128 cy) {
    // Compute mask (all F's for true, 0 for false)
    __m128 mask = _mm_and_ps(_mm_and_ps(_mm_cmpgt_ps(ay, fov),
                                  _mm_cmpgt_ps(by, fov)),
                        _mm_cmpgt_ps(cy, fov));

    // Convert mask to 1.0f (true) or 0.0f (false)
    return _mm_and_ps(mask, _mm_set1_ps(1.0f));
}
//sse render triangles
inline void render_tris_sse(int tris_amount, int start, int which, float* depth_buffer,int* colour_buffer) {

    int colour[3];
    int scr_size = cam.data[6]*cam.data[7];
    float aspect = cam.data[6]/cam.data[7]/2;

    __m128 aspects = _mm_setr_ps(aspect,aspect,aspect,aspect);
    float tris[36];
    float w1,w2;

    __m128 obj_size_X = _mm_set_ps(objects[which].trans[0],objects[which].trans[0],objects[which].trans[0],objects[which].trans[0]);
    __m128 obj_size_Y = _mm_set_ps(objects[which].trans[1],objects[which].trans[1],objects[which].trans[1],objects[which].trans[1]);
    __m128 obj_size_Z = _mm_set_ps(objects[which].trans[2],objects[which].trans[2],objects[which].trans[2],objects[which].trans[2]);

    __m128 obj_pos_X  = _mm_set_ps(objects[which].trans[6],objects[which].trans[6],objects[which].trans[6],objects[which].trans[6]);
    __m128 obj_pos_Y  = _mm_set_ps(objects[which].trans[7],objects[which].trans[7],objects[which].trans[7],objects[which].trans[7]);
    __m128 obj_pos_Z  = _mm_set_ps(objects[which].trans[8],objects[which].trans[8],objects[which].trans[8],objects[which].trans[8]);


    __m128 cam_pos_X  = _mm_set_ps(cam.data[3],cam.data[3],cam.data[3],cam.data[3]);
    __m128 cam_pos_Y  = _mm_set_ps(cam.data[4],cam.data[4],cam.data[4],cam.data[4]);
    __m128 cam_pos_Z  = _mm_set_ps(cam.data[5],cam.data[5],cam.data[5],cam.data[5]);

    __m128 cam_opening_X = _mm_set_ps(cam.data[6],cam.data[6],cam.data[6],cam.data[6]);
    __m128 cam_opening_Y = _mm_set_ps(cam.data[7],cam.data[7],cam.data[7],cam.data[7]);

    __m128 ax,ay,az,
           bx,by,bz,
           cx,cy,cz;

    __m128 Nvec_x,Nvec_y,Nvec_z,
           Npoint_x,Npoint_y,Npoint_z;

    __m128 culling;

    float Nvec[4][3];
        float Npoint[4][3];

    //so I can use multiplication further down
    float fov = 1/cam.data[8];
    __m128 cam_fov = _mm_set_ps(fov,fov,fov,fov);

    for (int i = 0; i < tris_amount/4; i++) {
        float* vert = &objects[which].vert[i*36+start  ];
        float* uv   = &objects[which].uv[i*24+start*6/9];

        ax = _mm_setr_ps(vert[0],vert[9 ],vert[18],vert[27]);
        ay = _mm_setr_ps(vert[1],vert[10],vert[19],vert[28]);
        az = _mm_setr_ps(vert[2],vert[11],vert[20],vert[29]);

        bx = _mm_setr_ps(vert[3],vert[12],vert[21],vert[30]);
        by = _mm_setr_ps(vert[4],vert[13],vert[22],vert[31]);
        bz = _mm_setr_ps(vert[5],vert[14],vert[23],vert[32]);

        cx = _mm_setr_ps(vert[6],vert[15],vert[24],vert[33]);
        cy = _mm_setr_ps(vert[7],vert[16],vert[25],vert[34]);
        cz = _mm_setr_ps(vert[8],vert[17],vert[26],vert[35]);

        ax = _mm_mul_ps(ax,obj_size_X);
        bx = _mm_mul_ps(bx,obj_size_X);
        cx = _mm_mul_ps(cx,obj_size_X);

        ay = _mm_mul_ps(ay,obj_size_Y);
        by = _mm_mul_ps(by,obj_size_Y);
        cy = _mm_mul_ps(cy,obj_size_Y);

        az = _mm_mul_ps(az,obj_size_Z);
        bz = _mm_mul_ps(bz,obj_size_Z);
        cz = _mm_mul_ps(cz,obj_size_Z);

        rot_3d(ax,ay,az,objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);
        rot_3d(bx,by,bz,objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);
        rot_3d(cx,cy,cz,objects[which].trans[3],objects[which].trans[5],objects[which].trans[4]);

        ax = _mm_add_ps(_mm_sub_ps(ax,cam_pos_X),obj_pos_X);
        ay = _mm_add_ps(_mm_sub_ps(ay,cam_pos_Y),obj_pos_Y);
        az = _mm_add_ps(_mm_sub_ps(az,cam_pos_Z),obj_pos_Z);

        bx = _mm_add_ps(_mm_sub_ps(bx,cam_pos_X),obj_pos_X);
        by = _mm_add_ps(_mm_sub_ps(by,cam_pos_Y),obj_pos_Y);
        bz = _mm_add_ps(_mm_sub_ps(bz,cam_pos_Z),obj_pos_Z);

        cx = _mm_add_ps(_mm_sub_ps(cx,cam_pos_X),obj_pos_X);
        cy = _mm_add_ps(_mm_sub_ps(cy,cam_pos_Y),obj_pos_Y);
        cz = _mm_add_ps(_mm_sub_ps(cz,cam_pos_Z),obj_pos_Z);

        rot_3d(ax,ay,az,cam.data[0],cam.data[1],cam.data[2]);
        rot_3d(bx,by,bz,cam.data[0],cam.data[1],cam.data[2]);
        rot_3d(cx,cy,cz,cam.data[0],cam.data[1],cam.data[2]);

        culling = cull(cam_fov,ay,by,cy);

        ax=_mm_mul_ps(ax,culling);
        ay=_mm_mul_ps(ay,culling);
        az=_mm_mul_ps(az,culling);

        bx=_mm_mul_ps(bx,culling);
        by=_mm_mul_ps(by,culling);
        bz=_mm_mul_ps(bz,culling);

        cx=_mm_mul_ps(cx,culling);
        cy=_mm_mul_ps(cy,culling);
        cz=_mm_mul_ps(cz,culling);

        az = _mm_mul_ps(az,aspects);
        bz = _mm_mul_ps(bz,aspects);
        cz = _mm_mul_ps(cz,aspects);


        __m128 Nvec_x;__m128 Nvec_y;__m128 Nvec_z;
        __m128 Npoint_x = ax;  // Using first vertex of each triangle
        __m128 Npoint_y = ay;
        __m128 Npoint_z = az;

        cross_prod(_mm_sub_ps(ax,cx),_mm_sub_ps(ay,cy),_mm_sub_ps(az,cz),
                   _mm_sub_ps(ax,bx),_mm_sub_ps(ay,by),_mm_sub_ps(az,bz),
                   Nvec_x,Nvec_y,Nvec_z);

        ay = _mm_mul_ps(ay,cam_fov);
        by = _mm_mul_ps(by,cam_fov);
        cy = _mm_mul_ps(cy,cam_fov);

        convert_mm(tris,ax,ay,az,bx,by,bz,cx,cy,cz,Nvec_x,Nvec_y,Nvec_z,Npoint_x,Npoint_y,Npoint_z,Nvec,Npoint);

        tris[0]=tris[0] /tris[1];
        tris[1]=tris[2] /tris[1];
        tris[2]=tris[3] /tris[4];
        tris[3]=tris[5] /tris[4];
        tris[4]=tris[6] /tris[7];
        tris[5]=tris[8] /tris[7];

        tris[0+9]=tris[0+9] /tris[1+9];
        tris[1+9]=tris[2+9] /tris[1+9];
        tris[2+9]=tris[3+9] /tris[4+9];
        tris[3+9]=tris[5+9] /tris[4+9];
        tris[4+9]=tris[6+9] /tris[7+9];
        tris[5+9]=tris[8+9] /tris[7+9];

        tris[0+18]=tris[0+18] /tris[1+18];
        tris[1+18]=tris[2+18] /tris[1+18];
        tris[2+18]=tris[3+18] /tris[4+18];
        tris[3+18]=tris[5+18] /tris[4+18];
        tris[4+18]=tris[6+18] /tris[7+18];
        tris[5+18]=tris[8+18] /tris[7+18];

        tris[0+27]=tris[0+27] /tris[1+27];
        tris[1+27]=tris[2+27] /tris[1+27];
        tris[2+27]=tris[3+27] /tris[4+27];
        tris[3+27]=tris[5+27] /tris[4+27];
        tris[4+27]=tris[6+27] /tris[7+27];
        tris[5+27]=tris[8+27] /tris[7+27];

        for (int a=0;a<4;a++){

        // Convert bounding box to pixel coordinates and clamp to screen bounds
        int bound_min_X = boost::algorithm::clamp((int)((std::min({tris[4+9*a], tris[0+9*a], tris[2+9*a]}) + 1) * 0.5 * cam.data[6]), 0, (int)cam.data[6]);
        int bound_max_X = boost::algorithm::clamp((int)((std::max({tris[4+9*a], tris[0+9*a], tris[2+9*a]}) + 1) * 0.5 * cam.data[6]), 0, (int)cam.data[6]-1);
        int bound_min_Y = boost::algorithm::clamp((int)((std::min({tris[5+9*a], tris[1+9*a], tris[3+9*a]}) + 1) * 0.5 * cam.data[7]), 0, (int)cam.data[7]);
        int bound_max_Y = boost::algorithm::clamp((int)((std::max({tris[5+9*a], tris[1+9*a], tris[3+9*a]}) + 1) * 0.5 * cam.data[7]), 0, (int)cam.data[7]);


        // Iterate over pixel coordinates
        for (int y = bound_min_Y; y <= bound_max_Y; y++) {
            float ny = (y / (float)cam.data[7]) * 2 - 1;
            for (int x = bound_min_X; x <= bound_max_X; x++) {
                // Convert pixel to normalized coordinates
                float nx = (x / (float)cam.data[6]) * 2 - 1;

                // Check if pixel is inside the triangle
                if (in_trig_barricentric(nx, ny, &tris[a*9], w1, w2)) {

                    int pixel_index = x + y * cam.data[6];
                    float Gvec[3]; Gvec[0]=nx;Gvec[1]=cam.data[8];Gvec[2]=ny;
                    float intersect[3];
                    inter_pl_str(Nvec[a],Npoint[a],Gvec,intersect);
                    float temp=sqrt(intersect[0]*intersect[0]+intersect[1]*intersect[1]+intersect[2]*intersect[2]);

                    if (pixel_index >= 0 && pixel_index < scr_size&&temp<depth_buffer[pixel_index]) {
                        depth_buffer[pixel_index]=temp;
                        find_texture_rgb(&textures[objects[which].text_p].texture[0],&objects[which].uv[i*24+a*6+start*6/9],w1,w2,
                                          textures[objects[which].text_p].dim_x,textures[objects[which].text_p].dim_y,colour);

                        memcpy(&colour_buffer[pixel_index*3],colour,3*sizeof(int));
                    }
                }
            }
        }
        }
    }
}

//mulit threaded sse rendering
//hyperthread the rendering
inline void render_tris_acell_sse(int o) {

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

            thread_pool.emplace_back(render_tris_sse, tris_per_thread, tris_per_thread * 9 * i,o,&depth_buffs[i][0],&colour_buffs[i][0]);
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



#endif // RENDER_SSE_H_INCLUDED
