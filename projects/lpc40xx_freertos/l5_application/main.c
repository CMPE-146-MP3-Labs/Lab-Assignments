#include "FreeRTOS.h"
#include "board_io.h"
#include "common_macros.h"
#include "ff.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

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

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

static void create_player_task(void) {
  // To avoid malloc, using xTaskCreateStatic() in place of xTaskCreate()
  static StackType_t player_task_stack[512 / sizeof(StackType_t)];
  static StaticTask_t player_task_struct;

  xTaskCreateStatic(player_task, "Player Task", ARRAY_SIZE(player_task_stack),
                    NULL, PRIORITY_LOW, player_task_stack, &player_task_struct);
}

static void create_reader_task(void) {

  xTaskCreate(reader_task, "Reader Task", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_LOW, NULL);
}

static void player_task(void *params) {
  char bytes_512[512];

  while (1) {
    xQueueReceive(Q_songdata, &bytes_512[0], portMAX_DELAY);
    for (int i = 0; i < sizeof(bytes_512); i++) {
      // while (!mp3_decoder_needs_data()) {
      //  vTaskDelay(1);
      //}

      fprintf(stderr, bytes_512[i]);
    }
  }
}

static void reader_task(void *params) {
  songname_t name;
  char bytes_512[512];
  UINT bytes_read;

  while (1) {
    xQueueReceive(Q_songname, &name[0], portMAX_DELAY);
    printf("Received song to play: %s\n", name);
    FIL mp3_file;

    f_open(&mp3_file, &name, FA_OPEN_EXISTING);
    while (!f_eof(&mp3_file)) {
      f_read(&mp3_file, &bytes_512, sizeof(bytes_512), &bytes_read);
      xQueueSend(Q_songdata, &bytes_512[0], portMAX_DELAY);
    }
    f_close(&mp3_file);
  }
}
