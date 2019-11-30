#ifndef DEBUG_H
#define DEBUG_H

#include "pm.h"

#define DEBUG_CHAR (*(volatile unsigned char *)(REG_BASE+0xD0))
#define DEBUG_UHEX8 (*(volatile unsigned char *)(REG_BASE+0xD1))
#define DEBUG_UINT8 (*(volatile unsigned char *)(REG_BASE+0xD2))
#define DEBUG_SINT8 (*(volatile unsigned char *)(REG_BASE+0xD3))
#define DEBUG_INT16L (*(volatile unsigned char *)(REG_BASE+0xD4))  // D6, too
#define DEBUG_UINT16 (*(volatile unsigned char *)(REG_BASE+0xD5))
#define DEBUG_SINT16 (*(volatile unsigned char *)(REG_BASE+0xD7))
#define DEBUG_FLOATL (*(volatile unsigned char *)(REG_BASE+0xDE))
#define DEBUG_SFLOAT (*(volatile unsigned char *)(REG_BASE+0xDF))

void dprintf(const char *fmt, ...);
void dprint_char(char c);
void dprint_str(char *str);
void dprint_byte(unsigned char b);
void dprint_uint(unsigned int i);
void dprint_sint(signed int i);

#endif
