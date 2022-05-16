/*

  VLSI Solution generic microcontroller example player / recorder definitions.
  v1.00.

  See VS10xx AppNote: Playback and Recording for details.

  v1.00 2012-11-23 HH  First release

*/
#ifndef PLAYER_RECORDER_H
#define PLAYER_RECORDER_H

#include "lpc40xx.h"
#include "ssp2.h"
#include "vs10xx_uc.h"

void VS1053PlayFile(uint8_t *bytes_512);
void init_mp3decoder();
void WriteSci(uint8_t addr, uint16_t data);
uint16_t ReadSci(uint8_t addr);
int WriteSdi(const uint8_t *data, uint8_t bytes);
void SaveUIState(void);
void RestoreUIState(void);
int GetUICommand(void);

#endif
