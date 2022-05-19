#include "FreeRTOS.h"
#include "ff.h"
#include "player.h"
#include "queue.h"
#include "sj2_cli.h"
#include "song_list.h"
#include "task.h"
#include <stdio.h>

// 'static' to make these functions 'private' to this file
static void create_player_task(void);
static void create_reader_task(void);
static void player_task(void *params);
static void reader_task(void *params);
typedef char songname_t[16];

QueueHandle_t Q_songname;
QueueHandle_t Q_mp3_command;

int main(void) {
  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_mp3_command = xQueueCreate(1, sizeof(int));
  create_reader_task();
  create_player_task();
  sj2_cli__init();

  song_list__populate();
  for (size_t song_number = 0; song_number < song_list__get_item_count();
       song_number++) {
    fprintf(stderr, "Song %2d: %s\n", (1 + song_number),
            song_list__get_name_for_item(song_number));
  }
  puts("Starting RTOS\n");
  vTaskStartScheduler();

  return 0;
}

static void create_player_task(void) {
  xTaskCreate(player_task, "Player Task", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);
}

static void create_reader_task(void) {
  xTaskCreate(reader_task, "Reader Task", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_LOW, NULL);
}

static void player_task(void *params) {
  VSTestInitHardware();
  int player_command = -1;

  while (1) {

    xQueueReceive(Q_mp3_command, &player_command, portMAX_DELAY);
    fprintf(stderr, "recieved: %d\n", player_command);
    /* Volume adjustment */
    int volLevel = ReadSci(SCI_VOL) & 0xFF;
    int bassLevel = (ReadSci(SCI_BASS) >> 12) & 0xF;
    int trebleLevel = (ReadSci(SCI_BASS) >> 4) & 0xF;
    fprintf(stderr, "VOL: %d\n", volLevel);
    switch (player_command) {
    case ('+'):
      if (volLevel < 255) {
        volLevel++;
        WriteSci(SCI_VOL, volLevel * 0x101);
        fprintf(stderr, "Volume lowered: %d", volLevel);
      }
      break;
    case ('-'):
      if (volLevel) {
        volLevel--;
        WriteSci(SCI_VOL, volLevel * 0x101);
        printf("Volume increased: %d", volLevel);
      }
      break;
    case ('p'):
      if (playerState == psPlayback)
        playerState = psUserRequestedCancel;
      if (playerState == psStopped)
        playerState = psPlayback;
      break;
    case ('s'):
      if (playerState == psPlayback)
        playerState = psUserRequestedCancel;
      break;
    case ('B'):
      if (bassLevel < 15) {
        bassLevel++;
        WriteSci(SCI_BASS, bassLevel * SB_AMPLITUDE);
        fprintf(stderr,"Bass lowered: %d", bassLevel);
      }
      break;
    case ('b'):
      if (bassLevel > 0) {
        bassLevel--;
        WriteSci(SCI_BASS, bassLevel * SB_AMPLITUDE);
        printf("Bass increased: %d", bassLevel);
      }
      break;
          case ('T'):
      if (trebleLevel < 7) {
        trebleLevel++;
        WriteSci(SCI_BASS, trebleLevel * ST_AMPLITUDE);
        fprintf(stderr,"Treble lowered: %d", trebleLevel);
      }
      break;
    case ('t'):
      if (trebleLevel > -8) {
        trebleLevel--;
        WriteSci(SCI_BASS, trebleLevel * ST_AMPLITUDE);
        printf("Treble increased: %d", trebleLevel);
      }
      break;
    }
  }
}

static void reader_task(void *params) {
  songname_t name;
  // char bytes_512[512];
  // UINT bytes_read;
  // FRESULT result;

  while (1) {
    xQueueReceive(Q_songname, &name[0], portMAX_DELAY);
    printf("Received song to play: %s\n", name);
    // FIL mp3_file;
    VSTestInitSoftware();
    VSTestHandleFile(&name, 0);

    /*result = f_open(&mp3_file, &name[0], FA_OPEN_EXISTING | FA_READ);
    fprintf(stderr, "Open Result: %i", result);



    while (!f_eof(&mp3_file)) {
      result = f_read(&mp3_file, &bytes_512, sizeof(bytes_512), &bytes_read);
      fprintf(stderr, "Read Result: %i", result);
      fprintf(stderr, "\nBytes read: %i\n", bytes_read);
      vTaskDelay(10);
      xQueueSend(Q_songdata, &bytes_512[0], portMAX_DELAY);
    }*/
    // f_close(&mp3_file);
  }
}
