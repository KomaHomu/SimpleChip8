#ifndef DEBUG_H
#define DEBUG_H

#include "chip8.h"

#include <stdio.h>

#define DEBUG_MODE 0

void log_opcode(uint16_t opcode, uint16_t pc);
void log_registers(Chip8* chip8);
void log_key_event(uint8_t key, int pressed);

#endif