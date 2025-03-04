#include "display.h"

#define SCREEN_SCALE 10
#define WINDOW_WIDTH (SCREEN_WIDTH * SCREEN_SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCREEN_SCALE)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_AudioDeviceID audio_device;
SDL_AudioSpec audio_spec;

void init_display()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void render_display(Chip8* chip8)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            if (chip8->display[y * SCREEN_WIDTH + x]) 
            {
                SDL_Rect pixel = { x * SCREEN_SCALE, y * SCREEN_SCALE, SCREEN_SCALE, SCREEN_SCALE };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void destroy_display()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void audio_callback(void* userdata, Uint8* stream, int len)
{
    for (int i = 0; i < len; i++)
    {
        stream[i] = (i % 2 == 0) ? 255 : 0;  // Simple square wave
    }
}

void init_sound_device()
{
    SDL_memset(&audio_spec, 0, sizeof(audio_spec));
    audio_spec.freq = 440;  // Frequency of beep
    audio_spec.format = AUDIO_U8;
    audio_spec.channels = 1;
    audio_spec.samples = 512;
    audio_spec.callback = audio_callback;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
}

void play_sound()
{
    SDL_PauseAudioDevice(audio_device, 0);
}

void stop_sound()
{
    SDL_PauseAudioDevice(audio_device, 1);
}
