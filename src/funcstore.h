#ifndef FUNCSTORE_MUTUAL_H_INCLUDED
#define FUNCSTORE_MUTUAL_H_INCLUDED

#include <boost/geometry.hpp>
#include <boost/algorithm/clamp.hpp>

using namespace std;

//creates the colour cube used for RGB-termial conversion
inline int rgb_to_terminal_color(int r, int g, int b) {
    r*=0.019;
    g*=0.019;
    b*=0.019;

    return 16 + r * 36 + g * 6 + b;
}

//creates colour pairs for ncurses to use (the colours of rgb_to_terminal_colours with black font)
inline void init_custom_colors() {
    // Preserve standard colors (0-15)
    // Initialize color cube at 16-231
    for (int r = 0; r < 6; r++) {
        for (int g = 0; g < 6; g++) {
            for (int b = 0; b < 6; b++) {
                int idx = 16 + r * 36 + g * 6 + b;
                init_color(idx,
                    r * 1000 / 5,  // Scale to 0-1000
                    g * 1000 / 5,
                    b * 1000 / 5
                );
            }
        }
    }
    for (int i=0;i<216;i++){
        for (int t=0;t<216;t++){
            init_pair(16+t,16+ i,16+t+i*216);
        }
    }
}

// 2D Rotation
inline void rot_2d(float& X, float& Y, const float angle) {
    float coa = cos(angle);float sia=sin(angle);
    float temp = X * coa- Y * sia;
    Y = X * sia + Y * coa;
    X = temp;
}
// 3D Rotiation
inline void rot_3d(float& X, float& Y, float& Z, const float ax, const float ay, const float az) {

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

//print screen from colour buffer
inline void draw_screen(std::string& screen) {
    for (int i = 0; i < screen.size(); i++) {
        int shade_index = (boost::algorithm::clamp(static_cast<int>(depth_buffer[i] / 10), 0, 9))* (depth_buffer[i] != INFINITY);
        screen[i] = shade[shade_index];

        attron(COLOR_PAIR(colour_buffer[i]* (depth_buffer[i] != INFINITY)+ 232* (depth_buffer[i] == INFINITY|A_DIM)));
        addch(' ');
    }
}
//draw a given texture on to screen
//just for viewing textures
inline void draw_texture(int which) {
    cam.data[2]=0;cam.data[0]=0;
    for (int y=0-cam.data[4]*10;y<cam.data[7]-cam.data[4]*10;y++){
        for (int x=0+cam.data[3]*10;x<cam.data[6]+cam.data[3]*10;x++){
            attron(COLOR_PAIR(textures[which].texture[(y*textures[which].dim_x+x)]));
            addch(' ');
        }
    }
}

//Controls SDL2 based
inline void controls(SDL_Event event,bool& quit){
    const Uint8 *key_state = SDL_GetKeyboardState(nullptr);
    float tempX=0;float tempY=0;
    tempX-=key_state[SDL_SCANCODE_A];
    tempX+=key_state[SDL_SCANCODE_D];

    tempY-=key_state[SDL_SCANCODE_S];
    tempY+=key_state[SDL_SCANCODE_W];

    rot_2d(tempX,tempY,-cam.data[2]);

    cam.data[5] -= mv_speed*key_state[SDL_SCANCODE_SPACE];
    cam.data[5] += mv_speed*key_state[SDL_SCANCODE_LSHIFT];
    cam.data[3] += tempX*mv_speed;
    cam.data[4] += tempY*mv_speed;
    //rotation control
    cam.data[0]+=rot_speed*key_state[SDL_SCANCODE_I];
    cam.data[0]-=rot_speed*key_state[SDL_SCANCODE_K];
    cam.data[2]+=rot_speed*key_state[SDL_SCANCODE_L];
    cam.data[2]-=rot_speed*key_state[SDL_SCANCODE_J];
    cam.data[1]+=rot_speed*key_state[SDL_SCANCODE_O];
    cam.data[1]-=rot_speed*key_state[SDL_SCANCODE_U];
    quit = key_state[SDL_SCANCODE_Q];
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEMOTION) {

            cam.data[2] += event.motion.xrel * rot_speed * 0.1f;
            cam.data[0] -= event.motion.yrel * rot_speed * 0.1f;
        }
    }
    if (key_state[SDL_SCANCODE_F3]) {
    settings[2] = !settings[2];  // Toggle the boolean value
    // Optional: Add a small delay or wait for key release to prevent rapid toggling
}
}

//controls Ncurses based
inline void ncurses_controls(bool& quit){
char input = getch();
    if(getch){
    quit = input=='q'||input=='Q';

    float tempX=0;float tempY=0;

    tempX-= (((float)input=='a')+((float)input=='A')*3);
    tempX+= (((float)input=='d')+((float)input=='D')*3);
    tempY-= (((float)input=='s')+((float)input=='S')*3);
    tempY+= (((float)input=='w')+((float)input=='W')*3);

    rot_2d(tempX,tempY,-cam.data[2]);

    cam.data[5] -= ((float)input=='+')*mv_speed;
    cam.data[5] += ((float)input=='-')*mv_speed;
    cam.data[3] += tempX*mv_speed;
    cam.data[4] += tempY*mv_speed;
    //rotation control
    cam.data[0]+=rot_speed*(((float)input=='i')+((float)input=='I')*3);
    cam.data[0]-=rot_speed*(((float)input=='k')+((float)input=='K')*3);
    cam.data[2]+=rot_speed*(((float)input=='l')+((float)input=='L')*3);
    cam.data[2]-=rot_speed*(((float)input=='j')+((float)input=='J')*3);
    cam.data[1]+=rot_speed*(((float)input=='o')+((float)input=='O')*3);
    cam.data[1]-=rot_speed*(((float)input=='u')+((float)input=='U')*3);
    }
}

//just like the youtube setting, it show useful information invoked with f3 (not usable with ncurses-controls)
inline void stats_4nerds(){
if(settings[2]){
        attron(COLOR_PAIR(231));
        string temp = to_string((frametime[0]+frametime[1]+frametime[2]+frametime[4]+frametime[4]+frametime[5]+frametime[6]+frametime[7]+frametime[8]+frametime[9])/10/1000)+"ms";
        mvprintw(0,0,temp.c_str());
        temp="res: "+to_string(win_size.X)+" x "+to_string(win_size.Y);
        mvprintw(1,0,temp.c_str());
        temp="angle: "+to_string(cam.data[0])+to_string(cam.data[1])+to_string(cam.data[2]);
        mvprintw(2,0,temp.c_str());

        int poly_am=0;
        int uv_am=0;
        for (int i=0;i<objects.size();i++){
            poly_am+=objects[i].vert.size();
            uv_am+=objects[i].uv.size();
        }
        temp="poly amount: "+to_string(poly_am/9);
        mvprintw(4,0,temp.c_str());
        temp="uv amount: "+to_string(uv_am/6);
        mvprintw(5,0,temp.c_str());
        temp="texture dim: x: "+to_string(textures[0].dim_x)+" y: "+to_string(textures[0].dim_y)+ " size:" +to_string(textures[0].texture.size());
        mvprintw(6,0,temp.c_str());

        }
}

//initalise things needed for the library
inline void innit_3d(){
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
    // Initialize ncurses
    initscr();

    raw();
    noecho(); // Disable input echoing
    cbreak();	//Line buffering disabled. pass on everything
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);   // Enable function keys, arrow keys, etc.

    start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	init_custom_colors();
}
//handle input per setting
inline void input(){
        // SDL activate / deactivate
        if (settings[0]){
        //makes the cusor stay in the window
        SDL_SetWindowInputFocus(window);
        controls(event,quit);
        }
        else{
            ncurses_controls(quit);
        }
}

//gets called at the begining of your frame
//screen resizing ,frame-sync and various buffer tasks
inline void begin_frame(){
        //begin clock
        timer = chrono::high_resolution_clock::now();

        // Retake screensize
        getmaxyx(stdscr, win_size.Y, win_size.X);

        //recreate buffers

        if(!(cam.data[6]==win_size.X&&cam.data[7]==win_size.Y)){
            // Add null checks before freeing
            if (depth_buffer) free(depth_buffer);
            if (colour_buffer) free(colour_buffer);

            cam.data[6] = win_size.X;
            cam.data[7] = win_size.Y;

            depth_buffer = (float*)malloc(cam.data[6] * cam.data[7] * sizeof(float));
            colour_buffer = (int*)malloc(cam.data[6] * cam.data[7] * sizeof(int));

            winsize_change = true;
        }
        fill_n(depth_buffer, cam.data[6] * cam.data[7], INFINITY);
        fill_n(colour_buffer, cam.data[6] * cam.data[7], 216);
}

//called at the end of every frame
//Makes ncurses print, creates avg framerate, sleeps and updates delta
inline void end_frame(){
        //blit screen
        refresh();
        //timer stuffs
        auto frame_duration = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - timer);
        frametime[delta % 10] = frame_duration.count();  // Convert to seconds
        delta++;
        usleep(std::clamp(static_cast<int>(framedelay - frame_duration.count()), 0, framedelay));
        //empty screen
        clear();
}
#endif // FUNCSTORE_MUTUAL_H_INCLUDED
