#ifndef FUNCSTORE_MUTUAL_H_INCLUDED
#define FUNCSTORE_MUTUAL_H_INCLUDED

#include "global.h"
#include "KonMath.h"

#include <boost/geometry.hpp>
#include <boost/algorithm/clamp.hpp>

inline void draw_pixel(int x,int y,char letter,
                       int background_r,int background_g,int background_b,
                       int foreground_r,int foreground_g,int foreground_b){

            auto& pixel = screen.PixelAt(x,y);
            pixel.character = letter;
            pixel.foreground_color = ftxui::Color::RGB(foreground_r,foreground_g,foreground_b);
            pixel.background_color = ftxui::Color::RGB(background_r,background_g,background_b);
}

//print screen from colour buffer
inline void draw_screen() {
    timer_blit = std::chrono::high_resolution_clock::now();
    for (int y=0;y<cam.data[7];y++){
        for (int x=0;x<cam.data[6];x++){
        auto& pixel = screen.PixelAt(x, y);
            pixel.background_color = ftxui::Color::RGB(colour_buffer[(int)(x+y*cam.data[6])*3  ]/colour_clamper*colour_clamper,
                                                       colour_buffer[(int)(x+y*cam.data[6])*3+1]/colour_clamper*colour_clamper,
                                                       colour_buffer[(int)(x+y*cam.data[6])*3+2]/colour_clamper*colour_clamper);
    }}
    frametime_blit[delta%10] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer_blit).count();
}

//draw a given texture on to screen
//just for viewing textures
inline void draw_texture(int which) {
    timer_blit = std::chrono::high_resolution_clock::now();
    cam.data[2]=0;cam.data[0]=0;
    for (int ay=0;ay<cam.data[7];ay++){
        for (int ax=0;ax<cam.data[6];ax++){
            int y = ay-cam.data[4]*10;
            int x = ax+cam.data[3]*10;

            auto& pixel = screen.PixelAt(ax, ay);
            pixel.character = ' ';
            pixel.foreground_color = ftxui::Color::White;
            pixel.background_color = ftxui::Color::RGB(textures[which].texture[(y*textures[which].dim_x+x)*3  ]/colour_clamper*colour_clamper,
                                                       textures[which].texture[(y*textures[which].dim_x+x)*3+1]/colour_clamper*colour_clamper,
                                                       textures[which].texture[(y*textures[which].dim_x+x)*3+2]/colour_clamper*colour_clamper);

        }
    }
    frametime_blit[delta%10] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer_blit).count();
}


//Controls SDL2 based
inline void controls(){
    const Uint8 *key_state = SDL_GetKeyboardState(nullptr);
    float tempX=0;float tempY=0;
    tempX+=key_state[SDL_SCANCODE_D]-key_state[SDL_SCANCODE_A];
    tempY+=key_state[SDL_SCANCODE_W]-key_state[SDL_SCANCODE_S];

    rotate_2d(tempX,tempY,-cam.data[2]);

    cam.data[5] += mv_speed*(key_state[SDL_SCANCODE_LSHIFT]-key_state[SDL_SCANCODE_SPACE]);
    cam.data[3] += tempX*mv_speed;
    cam.data[4] += tempY*mv_speed;
    //rotation control
    cam.data[0]+=rot_speed*(key_state[SDL_SCANCODE_I]-key_state[SDL_SCANCODE_K]);
    cam.data[2]+=rot_speed*(key_state[SDL_SCANCODE_L]-key_state[SDL_SCANCODE_J]);
    cam.data[1]+=rot_speed*(key_state[SDL_SCANCODE_O]-key_state[SDL_SCANCODE_U]);
    quit = key_state[SDL_SCANCODE_Q];
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEMOTION) {

            cam.data[2] += event.motion.xrel * rot_speed * 0.1f;
            cam.data[0] -= event.motion.yrel * rot_speed * 0.1f;
        }
    }

    if(key_state[SDL_SCANCODE_F3]){
    settings[2] = (!settings[2]);
    }
}

//just like the youtube setting, it show useful information invoked with f3
inline void stats_4nerds(){
if(settings[2]){
        float temporary=0;
        for(int i=0;i<10;i++){temporary+=frametime[i];}
        std::string temp ="frametime: "+std::to_string(temporary/10000)+"ms";
        for (int i=0;i<temp.size();i++){draw_pixel(i,0,temp[i],0,0,0,255,255,255);}

        temporary=0;
        for(int i=0;i<10;i++){temporary+=frametime_render[i];}
        temp = "render time: "+std::to_string(temporary/10000)+"ms";
        for (int i=0;i<temp.size();i++){draw_pixel(i,1,temp[i],0,0,0,255,255,255);}

        temporary=0;
        for(int i=0;i<10;i++){temporary+=frametime_blit[i];}
        temp = "blit time: "+std::to_string(temporary/10000)+"ms";
        for (int i=0;i<temp.size();i++){draw_pixel(i,2,temp[i],0,0,0,255,255,255);}



        temp="res: "+std::to_string(cam.data[6])+" x "+std::to_string(cam.data[7]);
        for (int i=0;i<temp.size();i++){draw_pixel(i,4,temp[i],0,0,0,255,255,255);}
        temp="angle: "+std::to_string(cam.data[0])+std::to_string(cam.data[1])+std::to_string(cam.data[2]);
        for (int i=0;i<temp.size();i++){draw_pixel(i,5,temp[i],0,0,0,255,255,255);}

        int poly_am=0;
        int uv_am=0;
        for (int i=0;i<objects.size();i++){
            poly_am+=objects[i].vert.size();
            uv_am+=objects[i].uv.size();
        }
        temp="poly amount: "+std::to_string(poly_am/9);
        for (int i=0;i<temp.size();i++){draw_pixel(i,6,temp[i],0,0,0,255,255,255);}
        temp="uv amount: "+std::to_string(uv_am/6);
        for (int i=0;i<temp.size();i++){draw_pixel(i,7,temp[i],0,0,0,255,255,255);}
        temp="texture dim: x: "+std::to_string(textures[0].dim_x)+" y: "+std::to_string(textures[0].dim_y)+ " size:" +std::to_string(textures[0].texture.size()/3);
        for (int i=0;i<temp.size();i++){draw_pixel(i,8,temp[i],0,0,0,255,255,255);}

        }
}

//initalise things needed for the library
inline void innit_konrender(){
    depth_buffs.resize(thread_numb);
    colour_buffs.resize(thread_numb);
if (settings[0]){
    //sdl innit
    SDL_Init(SDL_INIT_VIDEO);

    // Create a small, transparent, always-on-top window
    SDL_Window *window = SDL_CreateWindow("SDL Console Input",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080,
        SDL_WINDOW_FULLSCREEN);
    // Hide the cursor
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetWindowOpacity(window, 0.0f);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    }
}

//gets called at the begining of your frame
//screen resizing ,frame-sync and various buffer tasks
inline void begin_frame(){
    screen = ftxui::Screen::Create(
        ftxui::Dimension::Full(),   // Use full terminal width
        ftxui::Dimension::Full()
    );

        //begin clock
        timer = std::chrono::high_resolution_clock::now();

        //recreate buffers

        if(!(cam.data[6]==screen.dimx()&&cam.data[7]==screen.dimy())){
            // Add null checks before freeing
            if (depth_buffer) free(depth_buffer);
            if (colour_buffer) free(colour_buffer);
            if (light_buffer) free(light_buffer);

            cam.data[6] = screen.dimx();
            cam.data[7] = screen.dimy();

            depth_buffer = (float*)malloc(cam.data[6] * cam.data[7] * sizeof(float));
            light_buffer = (float*)malloc(cam.data[6] * cam.data[7] * sizeof(float));
            colour_buffer = (int*)malloc(3*cam.data[6] * cam.data[7] * sizeof(int));

            winsize_change = true;
        }
        std::fill_n(depth_buffer, cam.data[6] * cam.data[7], INFINITY);
        std::fill_n(light_buffer, cam.data[6] * cam.data[7], NAN);
        std::fill_n(colour_buffer, 3*cam.data[6] * cam.data[7], 0);
}

inline void blit_screen(ftxui::Screen screen){
    timer_blit = std::chrono::high_resolution_clock::now();
    //blit screen
    std::cout << "\033[H";
    screen.Print();
    frametime_blit[delta%10] += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer_blit).count();
}

//called at the end of every frame
//Makes FTXUI print, creates avg framerate, sleeps and updates delta
inline void end_frame(){

        screen_buffered=screen;

        timer_render = std::chrono::high_resolution_clock::now();

        delta++;

        frametime_render[delta%10]=0;
        frametime_blit[delta%10]=0;
        //timer stuffs
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer);
        frametime[delta % 10] = frame_duration.count();  // Convert to seconds
        usleep(std::clamp(static_cast<int>(framedelay - frame_duration.count()), 0, framedelay));
}
#endif // FUNCSTORE_MUTUAL_H_INCLUDED
