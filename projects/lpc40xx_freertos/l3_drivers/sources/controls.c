#include <string.h>
#include <stdio.h>
#include "controls.h"
#include <VS1053b.h>

static uint32_t volume_control(int mosi)
{
    uint32_t change = 0x0101;
    if(volume_up == 1 && volume_down == 1) //check if both buttons are pressed
    {
        ; // do nothing when both volume up and volume down buttons are pressed together
    }
    else
    {
        if(volume_down == 1 && (SCI_VOL + change) < 0xFEFF) // 0xFEFE is the total silence
        {
            SCI_VOL += change;
        }
        if(volume_up == 1 && (SCI_VOL - change) > 0x0000) //0x0000 is the maximum volume
        {
            SCI_VOL -= change;
        }
    }
}




typedef struct
{
    uint4_t ST_AMPLITUDE[4];
    uint4_t ST_FREQLIMIT[4];
    uint4_t SB_AMPLITUDE[4];
    uint4_t SB_FREQLIMIT[4];
}Bass_and_Treble;


static uint32_t bass_and_treble_control(uint4_t sb_amp_input, uint4_t st_amp_input)
{
    int lower_limit_freq_in_10_HZ_steps = 1;
    int lower_limit_freq_in_1000_HZ_steps = 1;
    Bass_and_Treble -> SB_AMPLITUDE |= sb_amp_input;
    Bass_and_Treble -> SB_FREQLIMIT = 1.5 * lower_limit_freq_in_10_HZ_steps;
    Bass_and_Treble -> ST_AMPLITUDE |= st_amp_input;
    Bass_and_Treble -> SB_FREQLIMIT = 1.5 * lower_limit_freq_in_1000_HZ_steps;
}






void SPI_task()
{
    const uint32_t spi_clock = 24;
    ssp2__init(spi_clock);
    while(1)
    {


        vTaskDelay(1000);
    }
}