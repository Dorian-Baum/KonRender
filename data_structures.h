#ifndef DATA-STRUCTURES_H_INCLUDED
#define DATA-STRUCTURES_H_INCLUDED

//legace code to be removed
// Vector and Triangle Classes
struct vec2d {
    float X, Y;
};


// Camera Class
struct camera_3d {
    // angle -> position -> opening -> fov
    float data[9] = {0,0,0,0,0,0,1,1,1.5};
};

// 3D Object Class
struct obj_3d {
    std::vector<float> vert; // Vertex data (9 per triangle)
    std::vector<float> uv; // Uv data 6 per triange
    //which texture
    int text_p;
    // scale position rotation that order 3 each xyz
    float trans[9]={1,1,1,-1.57,0,0,0,0,0};
};

// Textures
struct texture {
    //texture in terminal format
    std::vector<int> texture;
    //texture coordinates
    int dim_x,dim_y;
};

#endif // DATA-STRUCTURES_H_INCLUDED
