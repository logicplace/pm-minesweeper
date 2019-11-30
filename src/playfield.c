#include <stdint.h>
#include <string.h>
#include "playfield.h"
#include "wyhash16.h"

#define MASK 0x0F
#define MINE 0x10
#define CLICKED 0x20
#define FLAG 0x40
#define UNSURE 0x80
#define MARK 0xC0

struct field_data field;

void generate_field(uint16_t seed, uint8_t width, uint8_t height, uint8_t mines) {
	uint8_t i, x, y;
	uint16_t pos;
	uint16_t size = width * height;
	uint8_t *f = field.field;

	if (
		width < 1 || width > 24
		|| height < 1 || height > 15
		|| mines >= size
	) {
		// Bad inputs
		return;
	}

	field.seed = seed;
	field.width = width;
	field.height = height;
	field.mines = mines;

	// Prepare the field with mines
	memset(f, 0, MAX_FIELD);
	wh16seed(seed);

	for (i = 0; i < mines; ++i) {
		do {
			pos = wyhash16() % MAX_FIELD;
		} while(f[pos] & MINE);
		f[pos] |= MINE;
	}

	// Pre-calculate counts
	for (pos = 0, y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x, ++pos) {
			uint8_t up_down;
			uint8_t total;
			if (f[pos] & MINE)
				continue;

			// Count mines around (x,y)
			up_down = (y > 0) << 1 | (y < height - 1);
			total = (
				// Directly above
				(up_down & 2 && f[pos - width] & MINE)
				// Direclt below
				+ (up_down & 1 && f[pos + width] & MINE)
			);

			if (x > 0) {
				uint16_t base_1 = pos - 1;
				total += (
					// Directly left
					(!!(f[base_1] & MINE))
					// Upper left
					+ (up_down & 2 && f[base_1 - width] & MINE)
					// Lower left
					+ (up_down & 1 && f[base_1 + width] & MINE)
				);
			}
			if (x < width - 1) {
				uint16_t base_1 = pos + 1;
				total += (
					// Directly right
					(!!(f[base_1] & MINE))
					// Upper right
					+ (up_down & 2 && f[base_1 - width] & MINE)
					// Lower right
					+ (up_down & 1 && f[base_1 + width] & MINE)
				);
			}

			f[pos] = total;
		}
	}
}

void draw_1x1(volatile unsigned char *dest) {
	// Draw 1 cell to 1 tile
	// unclicked, blank, 1-8, mine, flag, question mark
	uint16_t i;
	uint16_t size;
	uint8_t *f = field.field;

	size = field.width * field.height;
	for (i = 0; i < size; ++i, ++f, ++dest) {
		if (*f & CLICKED) {
			if (*f & MINE) {
				*dest = T_MINE;
			}
			else {
				*dest = (*f & MASK) + T_NUMBERS;
			}
		}
		else switch (*f & MARK) {
			case FLAG:
				*dest = T_FLAG;
				break;
			case UNSURE:
				*dest = T_UNSURE;
				break;
			default:
				*dest = T_UNCLICKED;
		}
	}
}

uint8_t click(uint8_t x, uint8_t y) {
	// returns 0bLZ
	//   L = 1 if lost game
	//   Z = 1 if field was zero (so click around)
	uint16_t pos;

	if (x >= field.width || y >= field.height) {
		return 0;
	}

	pos = y * field.width + x;

	if (field.field[pos] & FLAG) {
		return 0;
	}

	field.field[pos] |= CLICKED;
	if (field.field[pos] & MINE) {
		return 2;
	}
	return !(field.field[pos] & MASK);
}

void flag(uint8_t x, uint8_t y) {
	uint16_t pos;

	if (x >= field.width || y >= field.height) {
		return;
	}

	pos = y * field.width + x;
	field.field[pos] += FLAG;
	if (!(field.field[pos] ^ MARK)) {
		// only two marks, so make 3rd be 0
		field.field[pos] ^= MARK;
	}
}

// TODO: jump to nearest unclicked in that direction
void up_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y) {
	if (y > 0) {
		*out_y = y - 1;
	}
}

void down_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y) {
	if (y < field.height - 1) {
		*out_y = y + 1;
	}
}
void left_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y) {
	if (x > 0) {
		*out_x = x - 1;
	}
}

void right_from(uint8_t x, uint8_t y, uint8_t *out_x, uint8_t *out_y) {
	uint8_t what = field.width - 1;
	if (x < what) {
		*out_x = x + 1;
	}
}
