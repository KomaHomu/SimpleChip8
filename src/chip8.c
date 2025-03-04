#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

void init(Chip8* chip8)
{
	memset(chip8, 0, sizeof(Chip8));
	chip8->pc = 0x200;
}

void load_rom(Chip8* chip8, const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		perror("Error loading ROM");
		exit(1);
	}
	fread(&chip8->memory[0x200], 1, MEMORY_SIZE - 0x200, file);
	fclose(file);
}

void cycle(Chip8* chip8)
{
	uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1]; // fetch
	chip8->pc += 2;

	if (DEBUG_MODE) 
	{
		log_opcode(opcode, chip8->pc);
		log_registers(chip8);
	}

	switch (opcode & 0xF000) // decodes & executes
	{
	case 0x0000:
		if (opcode == 0x00E0) // clear screen
		{
			memset(chip8->display, 0, sizeof(chip8->display));
		}
		else if (opcode == 0x00EE) { // return from sb.routines
			chip8->pc = chip8->stack[--chip8->sp];
		}
		break;
	case 0x1000: // jumps
		chip8->pc = opcode & 0x0FFF;
		break;
	case 0x2000: // calls sb.routines
		chip8->stack[chip8->sp++] = chip8->pc;
		chip8->pc = opcode & 0x0FFF;
		break;
	case 0x3000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t NN = opcode & 0x00FF;
		if (chip8->v_reg[X] == NN)
		{
			chip8->pc += 2;
		}
		break;
	}
	case 0x4000: 
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t NN = opcode & 0x00FF;
		if (chip8->v_reg[X] != NN)
		{
			chip8->pc += 2;
		}
		break;
	}
	case 0x5000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t Y = (opcode & 0x00F0) >> 4;
		if (chip8->v_reg[X] == chip8->v_reg[Y])
		{
			chip8->pc += 2;
		}
		break;
	}
	case 0x9000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t Y = (opcode & 0x00F0) >> 4;
		if (chip8->v_reg[X] != chip8->v_reg[Y])
		{
			chip8->pc += 2;
		}
		break;
	}
	case 0x6000: // sets VX to NN
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t NN = opcode & 0x00FF;
		chip8->v_reg[X] = NN;
		break;
	}
	case 0x7000: // adds NN to VX
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t NN = opcode & 0x00FF;
		chip8->v_reg[X] += NN;
		break;
	}		
	case 0x8000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		uint8_t Y = (opcode & 0x00F0) >> 4;


		switch (opcode & 0x000F)
		{
		case 0x0:
		{
			chip8->v_reg[X] = chip8->v_reg[Y];
			break;
		}
		case 0x1:
		{
			chip8->v_reg[X] |= chip8->v_reg[Y];
			break;
		}
		case 0x2:
		{
			chip8->v_reg[X] &= chip8->v_reg[Y];
			break;
		}
		case 0x3:
		{
			chip8->v_reg[X] ^= chip8->v_reg[Y];
			break;
		}
		case 0x4:
		{
			uint16_t sum = chip8->v_reg[X] + chip8->v_reg[Y];
			chip8->v_reg[0xF] = (sum > 255);
			chip8->v_reg[X] = sum & 0xFF;
			break;
		}
		case 0x5:
		{
			chip8->v_reg[0xF] = (chip8->v_reg[X] >= chip8->v_reg[Y]);
			chip8->v_reg[X] -= chip8->v_reg[Y];
			break;
		}
		case 0x7:
		{
			chip8->v_reg[0xF] = (chip8->v_reg[X] <= chip8->v_reg[Y]);
			chip8->v_reg[X] = chip8->v_reg[Y] - chip8->v_reg[X];
			break;
		}
		case 0x6:
		{
			chip8->v_reg[0xF] ^= chip8->v_reg[X] & 1;
			chip8->v_reg[X] >>= 1;
			break;
		}
		case 0xE:
		{
			chip8->v_reg[0xF] ^= chip8->v_reg[X] & 1;
			chip8->v_reg[X] <<= 1;
			break;
		}
		default:
			// error
			break;
		}
	}
		
	case 0xA000:
		chip8->I = opcode & 0x0FFF;
		break;

	case 0xB000:
		chip8->pc = (opcode & 0x0FFF) + chip8->v_reg[(opcode & 0x0F00) >> 8];
		break;
	
	case 0xC000: // generates a random number, binary ANDs it with NN, puts in VX
		chip8->v_reg[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
		break;

	case 0xD000: // display
	{
		uint8_t coordX = chip8->v_reg[(opcode & 0x0F00) >> 8];
		uint8_t coordY = chip8->v_reg[(opcode & 0x00F0) >> 4];
		uint8_t sprite_height = opcode & 0x000F;
		chip8->v_reg[0xF] = 0;
		for (int rows = 0; rows < sprite_height; rows++)
		{
			uint8_t sprite = chip8->memory[chip8->I + rows];
			for (int cols = 0; cols < 8; cols++)
			{
				if ((sprite & (0x80 >> cols)) != 0)
				{
					int idx = (coordX + cols) % SCREEN_WIDTH + ((coordY + rows) % SCREEN_HEIGHT) * SCREEN_WIDTH;
					if (chip8->display[idx])
					{
						chip8->v_reg[0xF] = 1;
					}
					chip8->display[idx] ^= 1;
				}
			}
		}
		break;
	}

	case 0xE000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		if ((opcode & 0x00FF) == 0x9E)
		{
			if (chip8->keypad[chip8->v_reg[X]])
			{
				chip8->pc += 2;
			}
		}
		else if ((opcode & 0x00FF) == 0xA1)
		{
			if (!chip8->keypad[chip8->v_reg[X]])
			{
				chip8->pc += 2;
			}
		}
		break;
	}

	case 0xF000:
	{
		uint8_t X = (opcode & 0x0F00) >> 8;
		switch (opcode & 0x00FF)
		{
		case 0x07: // timers
			chip8->v_reg[X] = chip8->delay_timer;
			break;

		case 0x15:
			chip8->delay_timer = chip8->v_reg[X];
			break;

		case 0x18:
			chip8->sound_timer = chip8->v_reg[X];
			break;

		case 0x1E: // add to I
			chip8->v_reg[0xF] = (chip8->I + chip8->v_reg[X] > 0x0FFF) ? 1 : 0;
			chip8->I += chip8->v_reg[X];
			break;

		case 0x0A:
		{
			int key_pressed = -1;
			for (int i = 0; i < 16; i++)
			{
				if (chip8->keypad[i])
				{
					key_pressed = i;
				}
			}
			if (key_pressed != -1)
			{
				chip8->pc -= 2;
			}
			else {
				chip8->v_reg[X] = key_pressed;
			}
			break;
		}

		case 0x29:
			chip8->I = chip8->v_reg[X] * 5;
			break;

		case 0x33:
		{
			uint8_t value = chip8->v_reg[X];
			chip8->memory[chip8->I] = value / 100;
			chip8->memory[chip8->I + 1] = (value / 10) % 10;
			chip8->memory[chip8->I + 2] = value % 10;
			break;
		}

		case 0x55:
			for (uint8_t i = 0; i <= X; i++)
			{
				chip8->memory[chip8->I + i] = chip8->v_reg[i];
			}
			break;

		case 0x65:
			for (uint8_t i = 0; i <= X; i++)
			{
				 chip8->v_reg[i] = chip8->memory[chip8->I + i];
			}
			break;

		default:
			break;
		}
		break;
	}

	default:
		// error
		break;
	}
}

int save_state(Chip8* chip8, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if (!file)
	{
		perror("Error saving state");
		return 0;
	}
	fwrite(chip8, sizeof(Chip8), 1, file);
	fclose(file);

	printf("State saved to %s\n", filename);
	return 1;
}

int load_state(Chip8* chip8, const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		perror("Error loading state");
		return 0;
	}
	fread(chip8, sizeof(Chip8), 1, file);
	fclose(file);

	printf("State loaded from %s\n", filename);
	return 1;
}
