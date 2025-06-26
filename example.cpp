#include "src/KonRender.h"

int main() {

    // initalise libraries
    innit_3d();

    //load 3d objects
    //should be relative paths
    import_obj("objects/Cube.obj","/your_path/");
    import_ppm("objects/Cube.ppm","/your_path/");

    //push back model, so it's not inside of you
    objects[0].trans[7]=60;
    //define his texture
    objects[0].text_p=0;

    while (!quit) {

        // Screen-space handling
        begin_frame();

        // Create a screen buffer
        string screen(cam.data[6] * cam.data[7], ' ');

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
        input();
        // Screen-space handling
        end_frame();
    }
    //end ncurses
    endwin();
    return 0;
}
