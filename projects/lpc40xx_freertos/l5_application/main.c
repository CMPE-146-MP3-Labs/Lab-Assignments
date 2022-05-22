#include "FreeRTOS.h"
#include "LCD_draw.h"
#include "common_macros.h"
#include "ff.h"
#include "player.h"
#include "queue.h"
#include "sj2_cli.h"
#include "song_list.h"
#include "ssd1309.h"
#include "task.h"
#include <stdio.h>

#define ACTUAL_BUTTONS 0

/*--------------MACROS-----------------------*/
#define cmd_play_pause 'p'
#define cmd_stop 's'
#define cmd_previous 'r'
#define cmd_next 'n'
#define cmd_bass_up 'B'
#define cmd_bass_down 'b'
#define cmd_trebble_up 'T'
#define cmd_trebble_down 't'
#define cmd_volume_up '+'
#define cmd_volume_down '-'
#define BUTTON0 0
#define BUTTON1 1
#define BUTTON2 2
#define BUTTON3 3
#define BUTTON4 4
#define BUTTON_UP 5
#define BUTTON_DOWN 6
// typedef char songname_t[32];
/*-------------------------------------------*/

/*-------------Task Functions----------------*/
static void create_player_task(void);
static void create_reader_task(void);
static void create_display_task(void);
static void create_button_task(void);
static void player_task(void *params);
static void reader_task(void *params);
static void display_task(void *params);
static void button_task(void *params);

static void create_uart_task(void);
static void uart_task(void *params);
/*-------------------------------------------*/

/*-------------Queue Handles-----------------*/
QueueHandle_t Q_songname;
QueueHandle_t Q_mp3_command;
QueueHandle_t Q_button;
/*-------------------------------------------*/

/*-------------Main--------------------------*/
int main(void) {
  Q_button = xQueueCreate(1, sizeof(uint8_t));
  Q_songname = xQueueCreate(1, sizeof(songname_t));
  Q_mp3_command = xQueueCreate(1, sizeof(int));

  create_reader_task();
  create_player_task();
  create_display_task();
  create_button_task();

  create_uart_task();

  /* Initialize the SPI interface */
  ssp2__initialize(24);
  /* Initialize the LCD display */
  LCD_Init();

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

static void create_display_task(void) {
  xTaskCreate(display_task, "display task", 2048 / sizeof(void *), 0, 1, 0);
}
static void create_button_task(void) {
  xTaskCreate(button_task, "button task", 1024 / sizeof(void *), 0, 1, 0);
}

static void create_player_task(void) {
  xTaskCreate(player_task, "Player Task", (512U * 8) / sizeof(void *), 0, 1, 0);
}

static void create_reader_task(void) {
  xTaskCreate(reader_task, "Reader Task", (512U * 8) / sizeof(void *), 0, 1, 0);
}

static void player_task(void *params) {
  VSTestInitHardware();
  char player_command = 0;
  int volLevel = ReadSci(SCI_VOL) & 0xFF;
  int bassLevel = (ReadSci(SCI_BASS) >> 12) & 0xF;
  int trebleLevel = (ReadSci(SCI_BASS) >> 4) & 0xF;

  while (1) {
    fprintf(stderr, "VOL: %d\n", volLevel);
    xQueueReceive(Q_mp3_command, &player_command, portMAX_DELAY);
    fprintf(stderr, "received: %c\n", player_command);
    switch (player_command) {
    case ('+'):
      if (volLevel > 0) {
        volLevel -= 40;
        if (volLevel < 15) {
          volLevel = 15;
        }
        WriteSci(SCI_VOL, volLevel * 0x101);
        fprintf(stderr, "Volume Increased: %d", volLevel);
      }
      break;
    case ('-'):
      if (volLevel < 256) {
        volLevel += 40;
        if (volLevel > 255) {
          volLevel = 255;
        }
        WriteSci(SCI_VOL, volLevel * 0x101);
        printf("Volume Decreased: %d", volLevel);
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
        fprintf(stderr, "Bass Increased: %d", bassLevel);
      }
      break;
    case ('b'):
      if (bassLevel > 0) {
        bassLevel--;
        WriteSci(SCI_BASS, bassLevel * SB_AMPLITUDE);
        printf("Bass Lowered: %d", bassLevel);
      }
      break;
    case ('T'):
      if (trebleLevel < 7) {
        trebleLevel++;
        WriteSci(SCI_BASS, trebleLevel * ST_AMPLITUDE);
        fprintf(stderr, "Treble Increased: %d", trebleLevel);
      }
      break;
    case ('t'):
      if (trebleLevel > -8) {
        trebleLevel--;
        WriteSci(SCI_BASS, trebleLevel * ST_AMPLITUDE);
        printf("Treble Lowered: %d", trebleLevel);
      }
      break;
    }
  }
}

static void reader_task(void *params) {
  songname_t name;
  // songname_t *first_song = song_list__get_name_for_item(1);

  // char bytes_512[512];
  // UINT bytes_read;
  // FRESULT result;

  // VSTestHandleFile(first_song, 0);

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

void display_task(void *params) {
  uint8_t button_pressed;
  uint8_t track_no;
  // songname_t name;
  char command;

  MP3_DISPLAY mp3_display;

  strcpy(mp3_display.current_song, "Wii_Theme.mp3");
  mp3_display.current_menu = 2; /* 0:main, 1:list, 2:settings */
  /** TODO: use read commands to get current value, but map them to 0-15 range
   * **/
  mp3_display.current_volume = 6;
  mp3_display.current_bass = 0;
  mp3_display.current_trebble = 0;
  mp3_display.current_song_page = 0;
  mp3_display.current_selection = 0;
  /** TODO: function to calculate cpu load? **/
  mp3_display.cpu_load = 49;

  draw_main_menu(&mp3_display);

  while (1) {
    xQueueReceive(Q_button, &button_pressed, portMAX_DELAY);
    switch (button_pressed) {
    case BUTTON0:
      if (mp3_display.current_menu == MENU_MAIN) {
        // Go to List Menu
        mp3_display.current_menu = MENU_LIST;
      } else {
        // Go to Main Menu
        mp3_display.current_menu = MENU_MAIN;
      }
      break;
    case BUTTON1:
      if (mp3_display.current_menu == MENU_MAIN) {
        // Previous Song
        command = cmd_previous;
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
      } else if (mp3_display.current_menu == MENU_LIST) {
        // Navigate Down List
        if (mp3_display.current_selection > 0) {
          mp3_display.current_selection -= 1;
        }
      } else if (mp3_display.current_menu == MENU_SETTINGS) {
        // Bass Decrease
        if (mp3_display.current_bass > 0) {
          mp3_display.current_bass -= 1;
          command = cmd_bass_down;
          xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
        }
      }
      break;
    case BUTTON2:
      if (mp3_display.current_menu == MENU_MAIN) {
        // Play/Pause song
        command = cmd_play_pause;
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
      } else if (mp3_display.current_menu == MENU_LIST) {
        // Navigate Up List
        if (mp3_display.current_selection < 3) {
          mp3_display.current_selection += 1;
        }
      } else if (mp3_display.current_menu == MENU_SETTINGS) {
        // Bass Increase
        if (mp3_display.current_bass < BASS_MAX) {
          mp3_display.current_bass += 1;
          command = cmd_bass_up;
          xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
        }
      }
      break;
    case BUTTON3:
      if (mp3_display.current_menu == MENU_MAIN) {
        // Next Song
        command = cmd_next;
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
      } else if (mp3_display.current_menu == MENU_LIST) {
        // Select Song
        /** TODO: handle track number or name to send.*/
        command = cmd_stop;
        track_no = ((mp3_display.current_song_page * 3) +
                    mp3_display.current_selection);
        strcpy(mp3_display.current_song,
               song_list__get_name_for_item(track_no));
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
        vTaskDelay(100);
        xQueueSend(Q_songname, song_list__get_name_for_item(track_no),
                   portMAX_DELAY);
      } else if (mp3_display.current_menu == MENU_SETTINGS) {
        // Trebble Decrease
        if (mp3_display.current_trebble > 0) {
          mp3_display.current_trebble -= 1;
          command = cmd_trebble_down;
          xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
        }
      }
      break;
    case BUTTON4:
      if (mp3_display.current_menu == MENU_MAIN) {
        // Go to Settings Menu
        mp3_display.current_menu = MENU_SETTINGS;
      } else if (mp3_display.current_menu == MENU_LIST) {
        // Next Page List
        if (mp3_display.current_song_page <
            3) { /* assuming only 3 pages : 12 songs */
          mp3_display.current_song_page += 1;
        } else {
          mp3_display.current_song_page = 0;
        }
      } else if (mp3_display.current_menu == MENU_SETTINGS) {
        // Trebble Increase
        if (mp3_display.current_trebble < TREBBLE_MAX) {
          mp3_display.current_trebble += 1;
          command = cmd_trebble_up;
          xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
        }
      }
      break;
    case BUTTON_UP:
      if (mp3_display.current_volume < 6) {
        // Volume Up
        mp3_display.current_volume += 1;
        command = cmd_volume_up;
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
      }
      break;
    case BUTTON_DOWN:
      if (mp3_display.current_volume > 0) {
        // Volume Down
        mp3_display.current_volume -= 1;
        command = cmd_volume_down;
        xQueueSend(Q_mp3_command, &command, portMAX_DELAY);
      }
      break;
    }
    if (mp3_display.current_menu == MENU_SETTINGS) {
      draw_settings_menu(&mp3_display);
    } else if (mp3_display.current_menu == MENU_LIST) {
      draw_list_menu(&mp3_display);
    } else {
      draw_main_menu(&mp3_display);
    }
  }
}

static void button_task(void *params) {
  uint8_t switch_press;
  /* actual switches */
#if ACTUAL_BUTTONS
  gpio_s button4 = gpio__construct_as_input(GPIO__PORT_1, 30);
  gpio_s button3 = gpio__construct_as_input(GPIO__PORT_1, 20);
  gpio_s button2 = gpio__construct_as_input(GPIO__PORT_1, 23);
  gpio_s button1 = gpio__construct_as_input(GPIO__PORT_1, 28);
  gpio_s button0 = gpio__construct_as_input(GPIO__PORT_1, 29);
  gpio_s vol_down = gpi_construct_as_input(GPIO__PORT_1, 25);
  gpio_s vol_up = gpio__construct_as_input(GPIO__PORT_1, 31);

  // gpio__set_function(button0, GPIO__FUNCITON_0_IO_PIN);
  while (true) {
    // *SW0
    if (!gpio__get(button0)) {
      printf("button 0");
      switch_press = 0;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW1
    if (!gpio__get(button1)) {
      printf("button 1");
      switch_press = 1;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW2
    if (!gpio__get(button2)) {
      printf("button 2");
      switch_press = 2;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW3
    if (!gpio__get(button3)) {
      printf("button 3");
      switch_press = 3;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW4
    if (!gpio__get(button4)) {
      printf("button 4");
      switch_press = 4;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW5 Volume Up
    if (!gpio__get(vol_up)) {
      printf("button VUP");
      switch_press = 5;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW6 Volume Down
    if (!gpio__get(vol_down)) {
      printf("button VDON");
      switch_press = 6;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    vTaskDelay(100);
  }

#else
  /*onboard switches for testing only*/
  gpio_s SW3 = gpio__construct_as_input(GPIO__PORT_0, 29);
  gpio_s SW2 = gpio__construct_as_input(GPIO__PORT_0, 30);
  gpio_s SW1 = gpio__construct_as_input(GPIO__PORT_1, 15);
  gpio_s SW0 = gpio__construct_as_input(GPIO__PORT_1, 19);

  while (true) {
    // TODO: If switch pressed, set the binary semaphore
    // *SW0
    if (gpio__get(SW3)) {
      switch_press = BUTTON0;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW5
    if (gpio__get(SW2)) {
      switch_press = BUTTON1;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW6
    if (gpio__get(SW1)) {
      switch_press = BUTTON2;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }
    // *SW4
    if (gpio__get(SW0)) {
      switch_press = BUTTON3;
      xQueueSend(Q_button, &switch_press, portMAX_DELAY);
    }

    // Task should always sleep otherwise they will use 100% CPU
    // This task sleep also helps avoid spurious semaphore give during switch
    // debeounce
    vTaskDelay(100);
  }

#endif
}

static void create_uart_task(void) {
  // It is advised to either run the uart_task, or the SJ2 command-line (CLI),
  // but not both Change '#if (0)' to '#if (1)' and vice versa to try it out
#if (0)
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif
}

// This sends periodic messages over printf() which uses system_calls.c to send
// them to UART0
static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  TickType_t ticks = 0;

  while (true) {
    // This loop will repeat at precise task delay, even if the logic below
    // takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 2000);

    /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire
     * output will be fully sent out before this function returns. See
     * system_calls.c for actual implementation.
     *
     * Use this style print for:
     *  - Interrupts because you cannot use printf() inside an ISR
     *    This is because regular printf() leads down to xQueueSend() that might
     * block but you cannot block inside an ISR hence the system might crash
     *  - During debugging in case system crashes before all output of printf()
     * is sent
     */
    ticks = xTaskGetTickCount();
    fprintf(stderr,
            "%u: This is a polled version of printf used for debugging ... "
            "finished in",
            (unsigned)ticks);
    fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

    /* This deposits data to an outgoing queue and doesn't block the CPU
     * Data will be sent later, but this function would return earlier
     */
    ticks = xTaskGetTickCount();
    printf("This is a more efficient printf ... finished in");
    printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
  }
}