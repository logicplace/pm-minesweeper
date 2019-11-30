#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <stdint.h>

#define T_UNCLICKED 0
#define T_NUMBERS 1
#define T_MINE 10
#define T_FLAG 11
#define T_UNSURE 12

// 24*16 is max PRC mode, -1 from height for the top bar
#define MAX_FIELD (24 * 15)

struct field_data {
	uint16_t seed;
	uint8_t width;
	uint8_t height;
	uint8_t mines;
	uint8_t field[MAX_FIELD];
};

void generate_field(uint16_t seed, uint8_t width, uint8_t height, uint8_t mines);
void draw_1x1(volatile unsigned char *dest);
uint8_t click(uint8_t x, uint8_t y);
void flag(uint8_t x, uint8_t y);

void up_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y);
void down_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y);
void left_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y);
void right_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y);

#endif //PLAYFIELD_H
