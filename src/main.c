#include "pm.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "playfield.h"
#include "playtiles.h"

int main(void)
{
    uint8_t keys;
    Field *myfield;
    
    PRC_MODE = COPY_ENABLE|SPRITE_ENABLE|MAP_ENABLE|MAP_24X16;
    PRC_RATE = RATE_36FPS;

    PRC_MAP = playtiles;
    
    for(;;) {
        TMR1_OSC = 0x13; // Use Oscillator 2 (31768Hz)
        TMR1_SCALE = 0x08 | 0x02 | 0x80 | 0x20; // Scale 2 (8192 Hz)
        TMR1_CTRL = 0x86; // Enable timer 2 at 0 in 16 bit mode
        wait_vsync();
        
        keys = ~KEY_PAD;
        if (keys & KEY_A) {
            new_field(myfield, (uint16_t) TMR1_CNT, 24, 15, 50);
            generate_field(myfield);
            draw_1x1(myfield, TILEMAP);
            TMR1_CTRL = 0; // Pause timer
        }
        if ((keys & KEY_UP) && (PRC_SCROLL_Y > 0)) {
            PRC_SCROLL_Y--;
        } else if ((keys & KEY_DOWN) && (PRC_SCROLL_Y < 31)) {
            PRC_SCROLL_Y++;
        }
        if ((keys & KEY_LEFT) && (PRC_SCROLL_X > 0)) {
            PRC_SCROLL_X--;
        } else if ((keys & KEY_RIGHT) && (PRC_SCROLL_X < 31)) {
            PRC_SCROLL_X++;
        }
    }
}
