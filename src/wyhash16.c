#include <stdint.h>

uint16_t wyhash16_x; 

// https://lemire.me/blog/2019/07/03/a-fast-16-bit-random-number-generator/
uint32_t hash16(uint32_t input, uint32_t key) {
  uint32_t hash = input * key;
  return ((hash >> 16) ^ hash) & 0xFFFF;
}

uint16_t wyhash16() {
  wyhash16_x += 0xfc15;  // must be odd
  return hash16(wyhash16_x, 0x2ab);
}

void wh16seed(uint16_t seed) {
	wyhash16_x = seed;
}
