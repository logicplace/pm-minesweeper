#include "debug.h"

#include <stdarg.h>
#include <stdint.h>

void dprintf(const char *fmt, ...) {
	va_list ap;
	const char *p;
	unsigned char i, cnt = 0;

	// Count %s
	for (p = fmt; *p; ++p) {
		if (*p == '%') {
			if (*(p+1) == '%') {
				++p;
			}
			else {
				++cnt;
			}
		}
	}

	va_start(ap, fmt);

	for (p = fmt; *p; ++p) {
		if (*p == '%') {
			switch(*++p) {
				case '%':
					DEBUG_CHAR = '%';
					break;
				case 'c':
					DEBUG_CHAR = va_arg(ap, char);
					break;
				case 's':
					dprint_str(va_arg(ap, char*));
					break;
				case 'x':
					DEBUG_UHEX8 = (uint8_t)va_arg(ap, uint16_t);
					break;
				case 'd': case 'i':
					dprint_sint(va_arg(ap, int16_t));
					break;
				case 'u':
					dprint_uint(va_arg(ap, uint16_t));
					break;
				// TODO: case 'f'
			}
		}
		else {
			DEBUG_CHAR = *p;
		}
	}

	va_end(ap);
}

void dprint_char(char c){
	DEBUG_CHAR = c;
}

void dprint_str(char *str) {
	char *pntr = str;
	while(*pntr) {
		DEBUG_CHAR = *pntr++;
	}
}

void dprint_byte(unsigned char b) {
	DEBUG_UHEX8 = b;
}

void dprint_uint(unsigned int i) {
	if (i < 256) {
		DEBUG_UINT8 = (unsigned char)i;
	}
	else {
		DEBUG_INT16L = (unsigned char)i;
		DEBUG_UINT16 = (unsigned char)(i >> 8);
	}
}

void dprint_sint(signed int i) {
	if (i < 256) {
		DEBUG_SINT8 = (unsigned char)i;
	}
	else {
		DEBUG_INT16L = (unsigned char)i;
		DEBUG_SINT16 = (unsigned char)(i >> 8);
	}
}
