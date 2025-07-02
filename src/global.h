#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define thread_numb 16

#define ncurses 0

#include <SDL2/SDL.h>
#include <chrono>

//prolly defined in SDL2 or ncurses
#undef border

#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

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

//so we can disable SDL
SDL_Window *window = nullptr;
SDL_Event event;

//string, with darkness wramp, for shadows
std::string shade = " .-:+*=?#&@";

std::vector<std::vector<int>> colour_buffs;
std::vector<std::vector<float>> depth_buffs;

//self explanatory
vec2d cur_pos;
float mv_speed = 0.5;
float rot_speed = 0.009;
bool quit = 0;
int delta = 0;

//to keep track of time
auto timer=std::chrono::high_resolution_clock::now();
auto timer_render=std::chrono::high_resolution_clock::now();
auto timer_blit=std::chrono::high_resolution_clock::now();

float frametime_render[10]={0,0,0,0,0,0,0,0,0,0};
float frametime_blit[10]={0,0,0,0,0,0,0,0,0,0};
float frametime[10]={0,0,0,0,0,0,0,0,0,0};

int framedelay=33333;

//ftxui
auto screen = ftxui::Screen::Create(
        ftxui::Dimension::Full(),   // Use full terminal width
        ftxui::Dimension::Full()
    );
#endif // GLOBAL_H_INCLUDED
