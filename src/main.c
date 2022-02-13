#include "pm.h"

#include <stdint.h>
#include <string.h>

#include "playfield.h"
#include "playtiles.h"
#include "libreidx_sprites.h"

#define released(k) (pkeys & k) && !(keys & k)

uint8_t gray = 0;
uint16_t map1, map2;

#define PRC_MAP_16 (*(volatile uint16_t *)(REG_BASE+0x82))

void vsync() {
    wait_vsync();
    PRC_MAP_16 = gray ? map1 : map2;
    gray ^= 1;
}

void scroll_to(oam_sprite_t *cursor, uint8_t x, uint8_t y) {
    uint8_t xp = x * 8;
    uint8_t yp = y * 8;
    uint8_t left = xp <= 11*8 ? 0 : xp - 11*8;
    uint8_t top = yp <= 6*8 ? 0 : yp - 6*8;
    uint8_t done = 0;

    // TODO: make cursor move nicely with scroll
    cursor->ctrl ^= OAM_ENABLE;

    while (done ^ 3) {
        if (xp < PRC_SCROLL_X) {
            --PRC_SCROLL_X;
        }
        else if (left > PRC_SCROLL_X) {
            ++PRC_SCROLL_X;
        }
        else {
            done |= 1;
        }

        if (yp < PRC_SCROLL_Y) {
            --PRC_SCROLL_Y;
        }
        else if (top > PRC_SCROLL_Y) {
            ++PRC_SCROLL_Y;
        }
        else {
            done |= 2;
        }

        vsync();
    }

    cursor->x = xp - PRC_SCROLL_X + 16;
    cursor->y = yp - PRC_SCROLL_Y + 24;
    cursor->ctrl |= OAM_ENABLE;
}

int main(void)
{
    uint8_t keys, pkeys, x, y, new_x, new_y, click_res;
    uint8_t started = 0;
    oam_sprite_t *cursor = OAM;
    
    PRC_MODE = COPY_ENABLE|SPRITE_ENABLE|MAP_ENABLE|MAP_24X16;
    PRC_RATE = RATE_36FPS;
    PRC_MAP = playtiles1;
    PRC_SPR = libreidx_sprites1;
    map1 = (uint16_t)playtiles1;
    map2 = (uint16_t)playtiles2;

    TMR1_OSC = 0x13; // Use Oscillator 2 (31768Hz)
    TMR1_SCALE = 0x08 | 0x02 | 0x80 | 0x20; // Scale 2 (8192 Hz)
    TMR1_CTRL = 0x86; // Enable timer 2 at 0 in 16 bit mode

    memset(TILEMAP, 1, 24 * 16);
    
    while (!started) {
        wait_vsync();

        keys = ~KEY_PAD;
        if (keys & KEY_A) {
            TMR1_CTRL = 0; // Pause timer
            generate_field((uint16_t) TMR1_CNT, 24, 15, 50);
            draw_1x1(TILEMAP + 24);
            started = 1;
        }
    }

    x = y = new_x = new_y = 0;
    cursor->x = 16;
    cursor->y = 24;
    cursor->tile = 0;
    cursor->ctrl = OAM_ENABLE;
    keys = 0;

    for (;;) {
        vsync();

        pkeys = keys;
        keys = ~KEY_PAD;

        if (released(KEY_A)) {
            click_res = click(x, y);
            if (click_res & 1) {
                click(x-1, y-1);
                click(x, y-1);
                click(x+1, y-1);
                click(x-1, y);
                click(x+1, y);
                click(x-1, y+1);
                click(x, y+1);
                click(x+1, y+1);
            }
            // TODO: deal with return value better
            draw_1x1(TILEMAP + 24);
        }

        if (released(KEY_B)) {
            flag(x, y);
            draw_1x1(TILEMAP + 24);
        }

        if (released(KEY_UP)) {
            up_from(x, y, &new_x, &new_y);
            if (new_y < y) {
                scroll_to(cursor, new_x, new_y);
                x = new_x;
                y = new_y;
            }
        } else if (released(KEY_DOWN)) {
            down_from(x, y, &new_x, &new_y);
            if (new_y > y) {
                scroll_to(cursor, new_x, new_y);
                x = new_x;
                y = new_y;
            }
        }
        if (released(KEY_LEFT)) {
            left_from(x, y, &new_x, &new_y);
            if (new_x < x) {
                scroll_to(cursor, new_x, new_y);
                x = new_x;
                y = new_y;
            }
        } else if (released(KEY_RIGHT)) {
            right_from(x, y, &new_x, &new_y);
            if (new_x > x) {
                scroll_to(cursor, new_x, new_y);
                x = new_x;
                y = new_y;
            }
        }
    }
}
