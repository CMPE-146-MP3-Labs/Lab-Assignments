/**********************************************************************
* $Id$    VS1053b.h      2012-12-03
*//**
* @file   VS1053b.h      
* @brief  VS1053b - Ogg Vorbis/MP3/AAC/WMA/FLAC/ MIDI AUDIO CODEC CIRCUIT
* @version  1.20
* @date   05. May 2022
* @author CMPE 146 - Team 8: 
*           Aaron Rice, Gianine Dao, Aishwar Gupta, Ulysses Villegas 
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************/

#ifndef __VS1053b_h__
#define __VS1053b_h__

#include "core_cm4.h" 

typedef struct
{
  __IO uint16_t SCI_MODE;                 // Mode control
  __IO uint16_t SCI_STATUS;               // Status of VS1053b
  __IO uint16_t SCI_BASS;                 // Built-in bass/treble control
  __IO uint16_t SCI_CLOCKF;               // Clock freq + multiplier
  __IO uint16_t SCI_DECODE_TIME;          // Decode time in seconds
  __IO uint16_t SCI_AUDATA;               // Misc. audio data
  __IO uint16_t SCI_WRAM;                 // RAM write/read
  __IO uint16_t SCI_WRAMADDR;             // Base address for RAM write/read
  __I  uint16_t SCI_HDAT0;                // Stream header data 0
  __I  uint16_t SCI_HDAT1;                // Stream header data 1
  __IO uint16_t SCI_AIADDR;               // Start address of application
  __IO uint16_t SCI_VOL;                  // Volume control
  __IO uint16_t SCI_AICTRL0;              // Application control register 0
  __IO uint16_t SCI_AICTRL1;              // Application control register 1
  __IO uint16_t SCI_AICTRL2;              // Application control register 2
  __IO uint16_t SCI_AICTRL3;              // Application control register 3

} VS_SCI_REG_TypeDef;

/*
*   SCI MODE is used to control the operation of VS1053b and defaults to 0x0800 (SM SDINEW set).
*/
typedef struct
{//Name                               //Function                        //Value/Description
  __IO uint16_t SM_DIFF;              // Differential                   0 normal in-phase audio   1 left channel inverted
  __IO uint16_t SM_LAYER12;           // Allow MPEG layers I & II       0 no                      1 yes
  __IO uint16_t SM_RESET;             // Soft reset                     0 no reset                1 reset
  __IO uint16_t SM_CANCEL;            // Cancel decoding current file   0 no                      1 yes
  __IO uint16_t SM_EARSPEAKER_LO;     // EarSpeaker low setting         0 off                     1 active
  __IO uint16_t SM_TESTS;             // Allow SDI tests                0 not allowed             1 allowed
  __IO uint16_t SM_STREAM;            // Stream mode                    0 no                      1 yes
  __IO uint16_t SM_EARSPEAKER_HI;     // EarSpeaker high setting        0 off                     1 active      
  __IO uint16_t SM_DACT;              // DCLK active edge               0 rising                  1 falling
  __IO uint16_t SM_SDIORD;            // SDI bit order                  0 MSb first               1 MSb last
  __IO uint16_t SM_SDISHARE;          // Share SPI chip select          0 no                      1 yes
  __IO uint16_t SM_SDINEW;            // VS1002 native SPI modes        0 no                      1 yes
  __IO uint16_t SM_ADPCM;             // ADPCM recording active         0 no                      1 yes
  __IO uint16_t reserved;             // reserved                       0 right                   1 wrong
  __IO uint16_t SM_LINE1;             // MIC / LINE1 selector           0 MICP                    1 LINE1
  __IO uint16_t SM_CLK_RANGE;         // Input clock range              0 12..13 MHz              1 24..26 MHz

} VS_SCI_MODE_TypeDef;

/*
*   SCI STATUS contains information on the current status of VS1053b.
*   It also controls some low-level things that the user does not usually have to care about.
*/
typedef struct
{
  __IO uint16_t SS_DO_NOT_JUMP;       // Header in decode, do not fast forward/rewind
  __IO uint16_t SS_SWING[3];          // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  __IO uint16_t SS_VCM_OVERLOAD;      // GBUF overload indicator ’1’ = overload
  __IO uint16_t SS_VCM_DISABLE;       // GBUF overload detection ’1’ = disable
  __IO uint16_t reserved[2];          // reserved
  __IO uint16_t SS_VER[4];            // Misc. audio data
  __IO uint16_t SS_APDOWN2;           // Analog driver powerdown
  __IO uint16_t SS_APDOWN1;           // Analog internal powerdown
  __IO uint16_t SS_AD_CLOCK;          // AD clock select, ’0’ = 6 MHz, ’1’ = 3 MHz
  __IO uint16_t SS_REFERENCE_SEL;     // Reference voltage selection, ’0’ = 1.23 V, ’1’ = 1.65 V

} VS_SCI_STATUS_TypeDef;

/*
*   The Bass Enhancer VSBE is a powerful bass boosting DSP algorithm, 
*   which tries to take the most out of the users earphones without causing clipping.
*/
typedef struct
{
  __IO uint16_t ST_AMPLITUDE[4];      // Header in decode, do not fast forward/rewind
  __IO uint16_t ST_FREQLIMIT[4];      // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  __IO uint16_t SB_AMPLITUDE[4];      // GBUF overload indicator ’1’ = overload
  __IO uint16_t SB_FREQLIMIT[4];      // GBUF overload detection ’1’ = disable

} VS_SCI_BASS_TypeDef;

/*
*   The operation of SCI CLOCKF has changed slightly in VS1053b compared to VS1003 and VS1033.
*   Multiplier 1.5× and addition 0.5× have been removed to allow higher clocks to be configured.
*/
typedef struct
{
  __IO uint16_t SC_MULT[3];           // Header in decode, do not fast forward/rewind
  __IO uint16_t SC_ADD[2];            // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  __IO uint16_t SC_FREQ[11];          // GBUF overload indicator ’1’ = overload

} VS_SCI_CLOCKF_TypeDef;



/*
*   SC DECODE TIME activates the built-in clock multiplier. 
*   This will multiply XTALI to create a higher CLKI.
*/




