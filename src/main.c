#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "chip8.h"
#include "display.h"
#include "input.h"
#include "debug.h"

#define CYCLE_DELAY 16  // Approx. 60Hz

void run(Chip8* chip8)
{
    init_display();
    init_sound_device();

    if (DEBUG_MODE) printf("Debug mode enabled\n");

    while (1)
    {
        handle_input(chip8);
        cycle(chip8);
        render_display(chip8);

        if (chip8->delay_timer > 0) chip8->delay_timer--;
        if (chip8->sound_timer > 0)
        {
            if (chip8->sound_timer == 1) play_sound();
            chip8->sound_timer--;
        }
        else
        {
            stop_sound();
        }

        SDL_Delay(CYCLE_DELAY);
    }

    destroy_display();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <ROM file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Chip8 chip8;
    init(&chip8);
    load_rom(&chip8, argv[1]);

    run(&chip8);

    return EXIT_SUCCESS;
}
