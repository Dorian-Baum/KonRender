#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define thread_numb 16

#include <ncurses.h>
#include <SDL2/SDL.h>
#include <chrono>

#include "data_structures.h"

//setting up data for optimization of functions
std::vector<obj_3d> objects;
std::vector<texture> textures;

//the camera
camera_3d cam;

float* depth_buffer=nullptr;
int* colour_buffer=nullptr;

//0 controls SDL/ncurses input
//1 controls nothing so far
//2 controls if stats for nerds is shown
bool settings[3] = {1,0,1};

//not yet used
bool winsize_change=true;

//to keep track of time
auto timer=std::chrono::high_resolution_clock::now();

//so we can disable SDL
SDL_Window *window = nullptr;
SDL_Event event;

//string, with darkness wramp, for shadows
std::string shade = " .-:+*=?#&@";

//self explanatory
vec2d cur_pos;
vec2d win_size;
float mv_speed = 0.5;
float rot_speed = 0.009;
bool quit = 0;
int delta = 0;
int framedelay=33333;
float frametime[10]={0,0,0,0,0,0,0,0,0,0};
#endif // GLOBAL_H_INCLUDED
