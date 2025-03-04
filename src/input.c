#include "input.h"
#include "debug.h"

#define SAVE_FILE "save_state.dat"

const SDL_Keycode keymap[16] = {
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

void handle_input(Chip8* chip8)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) exit(0);

        // handles save/load
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_F1)
            {
                save_state(chip8, SAVE_FILE);
            }
            else if (event.key.keysym.sym == SDLK_F2)
            {
                load_state(chip8, SAVE_FILE);
            }
        }

        // handles keypad input
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            uint8_t is_pressed = (event.type == SDL_KEYDOWN);
            for (int i = 0; i < 16; i++)
            {
                if (event.key.keysym.sym == keymap[i])
                {
                    chip8->keypad[i] = is_pressed;
                    log_key_event(i, is_pressed);
                }
            }
        }
    }
}