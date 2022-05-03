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
  __IO uint16_t MODE;                 // Mode control
  __IO uint16_t STATUS;               // Status of VS1053b
  __IO uint16_t BASS;                 // Built-in bass/treble control
  __IO uint16_t CLOCKF;               // Clock freq + multiplier
  __IO uint16_t DECODE_TIME;          // Decode time in seconds
  __IO uint16_t AUDATA;               // Misc. audio data
  __IO uint16_t WRAM;                 // RAM write/read
  __IO uint16_t WRAMADDR;             // Base address for RAM write/read
  __I  uint16_t HDAT0;                // Stream header data 0
  __I  uint16_t HDAT1;                // Stream header data 1
  __IO uint16_t AIADDR;               // Start address of application
  __IO uint16_t VOL;                  // Volume control
  __IO uint16_t AICTRL0;              // Application control register 0
  __IO uint16_t AICTRL1;              // Application control register 1
  __IO uint16_t AICTRL2;              // Application control register 2
  __IO uint16_t AICTRL3;              // Application control register 3

} VS_SCI_REG_TypeDef;

/*
*   SCI_STATUS contains information on the current status of VS1053b. 
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
  __IO  uint16_t SS_AD_CLOCK;         // AD clock select, ’0’ = 6 MHz, ’1’ = 3 MHz
  __IO  uint16_t SS_REFERENCE_SEL;    // Reference voltage selection, ’0’ = 1.23 V, ’1’ = 1.65 V

} VS_SCI_STATUS_TypeDef;

typedef struct
{
  __IO uint16_t ST_AMPLITUDE[4];      // Header in decode, do not fast forward/rewind
  __IO uint16_t ST_FREQLIMIT[4];      // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  __IO uint16_t SB_AMPLITUDE[4];      // GBUF overload indicator ’1’ = overload
  __IO uint16_t SB_FREQLIMIT[4];      // GBUF overload detection ’1’ = disable

} VS_SCI_BASS_TypeDef;

typedef struct
{
  __IO uint16_t SC_MULT[3];           // Header in decode, do not fast forward/rewind
  __IO uint16_t SC_ADD[2];            // Set swing to +0 dB, +0.5 dB, .., or +3.5 dB
  __IO uint16_t SC_FREQ[11];          // GBUF overload indicator ’1’ = overload

} VS_SCI_CLOCKF_TypeDef;






