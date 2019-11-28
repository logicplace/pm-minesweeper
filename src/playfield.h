#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <stdint.h>

#define T_UNCLICKED 0
#define T_NUMBERS 1
#define T_MINE 10
#define T_FLAG 11
#define T_UNSURE 12

typedef struct field {
	uint16_t seed;
	uint8_t width;
	uint8_t height;
	uint8_t mines;
	uint8_t *field;
} Field;

#define new_field(f, s, w, h, m) \
	f = (Field*)malloc(sizeof(Field)); \
	f->seed = s; \
	f->width = w; \
	f->height = h; \
	f->mines = m; \
	f->field = 0;

#define destroy_field(f) \
	if (f) { \
		if(f->field) free(f->field); \
		free(f); \
	}

void generate_field(Field *field);
void draw_1x1(Field *field, volatile unsigned char *dest);

#endif //PLAYFIELD_H
