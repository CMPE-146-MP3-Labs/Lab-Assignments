#pragma once

#include <stdint.h>
#include <stdlib.h>

static uint32_t volume_control(int mosi);

static uint32_t bass_and_treble_control(uint4_t sb_amp_input, uint4_t st_amp_input);

void SPI_task();