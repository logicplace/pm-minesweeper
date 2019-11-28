TOOLCHAIN_DIR := ../..
TARGET = minesweeper

C_SOURCES = src/isr.c src/main.c src/playfield.c src/wyhash16.c
ASM_SOURCES = src/startup.asm

include $(TOOLCHAIN_DIR)/pm.mk
