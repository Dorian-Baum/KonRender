#include "src/KonRender.h"

int main() {

    // initalise libraries
    innit_3d();

    //load 3d objects
    import_obj("objects/Cube.obj","/yourpath/");
    import_ppm("objects/Cube.ppm","/yourpath/");

    //push back model, so it's not inside of you
    objects[0].trans[7]=60;
    //define his texture
    objects[0].text_p=0;

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
