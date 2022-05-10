#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "ff.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "sj2_cli.h"
#include "song_list.h"
#include "task.h"
#include "ssp2.h"
#include <stdio.h>
#include <stdlib.h>
#include "controls.h"

// 'static' to make these functions 'private' to this file
static void create_player_task(void);
static void create_reader_task(void);
static void player_task(void *params);
static void reader_task(void *params);
typedef char songname_t[16];

QueueHandle_t Q_songname;
QueueHandle_t Q_songdata;

int main(void) {
  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_songdata = xQueueCreate(1, 512);
  create_reader_task();
  create_player_task();
  sj2_cli__init();

  puts("Starting RTOS\n");

  song_list__populate();
  for (size_t song_number = 0; song_number < song_list__get_item_count();
       song_number++) {
    printf("Song %2d: %s\n", (1 + song_number),
           song_list__get_name_for_item(song_number));
  }

  vTaskStartScheduler();

  return 0;
}

static void create_player_task(void) {
  xTaskCreate(player_task, "Player Task", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_LOW, NULL);
}

static void create_reader_task(void) {
  xTaskCreate(reader_task, "Reader Task", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);
}

static void player_task(void *params) {
  char bytes_512[512] = {0};
  ssp2__initialize(400);

  while (1) {
    while (uxQueueMessagesWaiting(Q_songdata) == 0) {
      vTaskDelay(1);
    }

    xQueueReceive(Q_songdata, &bytes_512[0], portMAX_DELAY);
    fprintf(stderr, "Output: ");
    for (int i = 0; i < sizeof(bytes_512); i++) {
      fprintf(stderr, "%c", bytes_512[i]);
    }
    vTaskDelay(2000);
  }
}

static void reader_task(void *params) {
  songname_t name;
  char bytes_512[512];
  UINT bytes_read;
  FRESULT result;

  while (1) {
    xQueueReceive(Q_songname, &name[0], portMAX_DELAY);
    printf("Received song to play: %s\n", name);
    FIL mp3_file;

    result = f_open(&mp3_file, &name[0], FA_OPEN_EXISTING | FA_READ);
    fprintf(stderr, "Open Result: %i", result);

    while (!f_eof(&mp3_file)) {
      result = f_read(&mp3_file, &bytes_512, sizeof(bytes_512), &bytes_read);
      fprintf(stderr, "Read Result: %i", result);
      fprintf(stderr, "\nBytes read: %i\n", bytes_read);
      vTaskDelay(10);
      xQueueSend(Q_songdata, &bytes_512[0], portMAX_DELAY);
    }
    f_close(&mp3_file);
  }
}
