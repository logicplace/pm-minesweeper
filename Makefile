TARGET = minesweeper

C_SOURCES = src\isr.c src\main.c src\playfield.c src\wyhash16.c
ASM_SOURCES = src\startup.asm #src\cursor.asm
IMAGES = rsc/sprites.png rsc/playtiles.png

OBJS = $(IMAGES:.png=.obj)

include ../../pm.mk

.SUFFIXES: .png
.png.obj:
    py img2c.py -O src $<
