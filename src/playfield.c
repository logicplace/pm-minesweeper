#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "playfield.h"
#include "wyhash16.h"

// Fit into 12x8 at a time

// 24*16 is max PRC mode, -1 from height for the top bar
#define MAX_FIELD (24 * 15)

#define MASK 0x0F
#define MINE 0x10
#define CLICKED 0x20
#define FLAG 0x40
#define UNSURE 0x80
#define MARK 0xC0

void generate_field(Field *field) {
	uint8_t i, x, y;
	uint16_t pos;
	uint16_t size = field->width * field->height;

	if (
		field->width < 1 || field->width > 24
		|| field->height < 1 || field->height > 15
		|| field->mines >= size
	) {
		// Bad inputs
		return;
	}

	// Prepare the field with mines
	if (field->field)
		free(field->field);
	field->field = (uint8_t*)calloc(size, 1);
	wh16seed(field->seed);

	for (i = 0; i < field->mines; ++i) {
		do {
			pos = wyhash16() % MAX_FIELD;
		} while(field->field[pos] & MINE);
		field->field[pos] |= MINE;
	}

	// Pre-calculate counts
	for (pos = 0, y = 0; y < field->height; ++y) {
		for (x = 0; x < field->width; ++x, ++pos) {
			uint8_t up_down;
			uint8_t total;
			if (field->field[pos] & MINE)
				continue;

			// Count mines around (x,y)
			up_down = (y > 0) << 1 | (y < field->height - 1);
			total = (
				// Directly above
				(up_down & 2 && field->field[pos - field->width] & MINE)
				// Direclt below
				+ (up_down & 1 && field->field[pos + field->width] & MINE)
			);

			if (x > 0) {
				uint16_t base_1 = pos - 1;
				total += (
					// Directly left
					(field->field[base_1] & MINE)
					// Upper left
					+ (up_down & 2 && field->field[base_1 - field->width] & MINE)
					// Lower left
					+ (up_down & 1 && field->field[base_1 + field->width] & MINE)
				);
			}
			if (x < field->width - 1) {
				uint16_t base_1 = pos + 1;
				total += (
					// Directly right
					(field->field[base_1] & MINE)
					// Upper right
					+ (up_down & 2 && field->field[base_1 - field->width] & MINE)
					// Lower right
					+ (up_down & 1 && field->field[base_1 + field->width] & MINE)
				);
			}

			field->field[pos] = total;
		}
	}
}

void draw_1x1(Field *field, volatile unsigned char *dest) {
	// Draw 1 cell to 1 tile
	// unclicked, blank, 1-8, mine, flag, question mark
	uint16_t i;
	uint16_t size;
	uint8_t *f = field->field;

	if (field->field == NULL) {
		// Bad input
		return;
	}

	size = field->width * field->height;
	for (i = 0; i < size; ++i, ++field, ++dest) {
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