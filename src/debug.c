#include "debug.h"

void log_opcode(uint16_t opcode, uint16_t pc)
{
    if (DEBUG_MODE)
        printf("[DEBUG] Executing opcode: 0x%04X (PC: 0x%03X)\n", opcode, pc);
}

void log_registers(Chip8* chip8)
{
    if (DEBUG_MODE)
    {
        printf("[DEBUG] Registers: ");
        for (int i = 0; i < REGISTERS_COUNT; i++)
            printf("V[%X]=%02X ", i, chip8->v_reg[i]);
        printf("I=%03X\n", chip8->I);
    }
}

void log_key_event(uint8_t key, int pressed)
{
    if (DEBUG_MODE)
        printf("[DEBUG] Key %X %s\n", key, pressed ? "Pressed" : "Released");
}
