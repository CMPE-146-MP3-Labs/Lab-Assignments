/*

  VLSI Solution generic microcontroller example player / recorder definitions.
  v1.00.

  See VS10xx AppNote: Playback and Recording for details.

  v1.00 2012-11-23 HH  First release

*/
#ifndef PLAYER_RECORDER_H
#define PLAYER_RECORDER_H

#include "lpc40xx.h"
#include "vs10xx_uc.h"

int VSTestInitHardware(void);
int VSTestInitSoftware(void);
int VSTestHandleFile(const char *fileName);
void init_mp3decoder() {
  int VSTestInitHardware(void);
  int VSTestInitSoftware(void);
}
void WriteSci(u_int8 addr, u_int16 data) {
  LPC_SSP2->DR = data;
  gpio__set(VS_CS);
  ssp2__dma_write_block(&addr, 2);
  gpio__reset(VS_CS);
}
u_int16 ReadSci(u_int8 addr) {
  gpio__set(VS_CS);
  ssp2__dma_read_block(&addr, 2);
  gpio__reset(VS_CS);
  return (uint16_t)(LPC_SSP2->DR & 0xFF);
}
int WriteSdi(const u_int8 *data, u_int8 bytes) {
  LPC_SSP2->DR = data;
  gpio__set(VS_CS);
  ssp2__dma_write_block(&bytes, 1);
  gpio__reset(VS_CS);
}
void SaveUIState(void) { return; }
void RestoreUIState(void) { return; }
int GetUICommand(void) { return 0; }

#endif
