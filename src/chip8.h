#ifndef CHIP8_H

#define CHIP8_H

#include <stdint.h>

#define MEMORY_SIZE 4096
#define REGISTERS_COUNT 16
#define STACK_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct 
{
	uint8_t memory[MEMORY_SIZE];
	uint16_t pc;
	uint8_t v_reg[REGISTERS_COUNT]; // register
	uint16_t I; // index register
	uint16_t stack[STACK_SIZE]; // stack
	uint8_t sp; // stack pointer
	uint8_t delay_timer;
	uint8_t sound_timer;
	uint8_t keypad[16];
	uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
} Chip8;

void init(Chip8* chip8);
void load_rom(Chip8* chip8, const char *filename);
void cycle(Chip8* chip8);
int save_state(Chip8* chip8, const char* filename);
int load_state(Chip8* chip8, const char* filename);

#endif