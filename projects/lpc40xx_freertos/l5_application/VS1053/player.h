/*

  VLSI Solution generic microcontroller example player / recorder definitions.
  v1.00.

  See VS10xx AppNote: Playback and Recording for details.

  v1.00 2012-11-23 HH  First release

*/
#ifndef PLAYER_RECORDER_H
#define PLAYER_RECORDER_H

#include "vs10xx_uc.h"

#define VS_XRESET gpio__construct_as_output(GPIO__PORT_2, 5)
#define VS_CS gpio__construct_as_output(GPIO__PORT_2, 2)
#define VS_DCS gpio__construct_as_output(GPIO__PORT_2, 6)
#define VS_DREQ gpio__construct_as_input(GPIO__PORT_2, 4)

enum PlayerStates {
  psPlayback = 0,
  psUserRequestedCancel,
  psCancelSentToVS10xx,
  psStopped
} playerState;

typedef char songname_t[16];

int VSTestInitHardware(void);
int VSTestInitSoftware(void);
int VSTestHandleFile(songname_t *fileName, int record);

void WriteSci(u_int8 addr, u_int16 data);
u_int16 ReadSci(u_int8 addr);
int WriteSdi(const u_int8 *data, u_int8 bytes);
void SaveUIState(void);
void RestoreUIState(void);

#endif
