#include "src/KonRender.h"

int main() {

    // initalise libraries
    innit_3d();

    //load 3d objects
    import_obj("objects/Cat.obj","/home/drschizo/Documents/Programming/c++/KonRender/KonRender/");
    import_ppm("objects/Cat.ppm","/home/drschizo/Documents/Programming/c++/KonRender/KonRender/");

    //define his texture
    objects[0].text_p=0;

    //push back camera, so that model is not inside you
    cam.data[4]=-50;

    while (!quit) {

        // Screen-space handling
        begin_frame();

        // Transform objects
        objects[0].trans[5]+=0.01;

        // Render all objects
        for (int i=0;i<objects.size();i++){
            render_tris_acell_sse(i);
        }

        // Create screen string from depth and colour buffers
        draw_screen();

        // As the Name suggests
        stats_4nerds();

        // Same as name
        controls();

        // Screen-space handling
        end_frame();
    }

    return 0;
}
