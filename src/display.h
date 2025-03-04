#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include "chip8.h"

void init_display();
void render_display(Chip8* chip8);
void destroy_display();
void init_sound_device();
void play_sound();
void stop_sound();

#endif