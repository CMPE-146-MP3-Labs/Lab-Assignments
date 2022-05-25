#pragma once
#include "ssd1309.h"
#include <stdio.h>
#include <string.h>

/* Macros */
#define UP 1
#define DOWN 0
#define VOL_MAX 6
#define VOL_MIN 0
#define BASS_MAX 15
#define TREBBLE_MAX 15

#define MENU_MAIN 0
#define MENU_LIST 1
#define MENU_SETTINGS 2

/* LCD Display Data Structure */
typedef struct {
  uint8_t current_menu;
  uint8_t current_volume;
  uint8_t current_bass;
  uint8_t current_trebble;
  uint8_t cpu_load;
  uint8_t current_song_page;
  uint8_t current_selection;
  char current_song[128];
} MP3_DISPLAY;

/* draw volume symbol */
void draw_volume_on(void);

/* draw volume symbol */
void draw_volume_mute(void);

/* draw play/pause symbol */
void draw_play_pause(void);

/* draw next/skip */
void draw_next_skip(void);

/* draw previous/rewind */
void draw_previous_rewind(void);

/* draw equalizer symbol */
void draw_settings(void);

/** draw X 8-bit ON/OFF columns
 * @param number : number of columns to draw
 * @param type : ON or OFF ( 1 or 0)
 */
void draw_columns(uint16_t number, uint8_t type);

/** draw volume bar
 * @param current_volume : value 0 - 6, this is normally retrieved from the
 * current MP3_DISPLAY context
 */
void draw_volume(uint8_t current_volume);

/* draw hamburger icon */
void draw_hamburger(void);

/* draw exit/return icon */
void draw_return_icon(void);

/** Writes the input word to the LCD
 * @param char_string : string or pointer to a char array
 */
void LCD_Write_String(const char *char_string);

/* draw up icon */
void draw_up_icon(void);

/* draw up icon */
void draw_down_icon(void);

/** Draws menu icons and current mp3 data
 * @param mp3 : current MP3_display context
 */
void draw_main_menu(MP3_DISPLAY *mp3);

/** Draws the static settings icons
 *  @param mp3 : current MP3_display context
 */
void draw_settings_menu(MP3_DISPLAY *mp3);

/** Draws the static list icons
 *  @param mp3 : current MP3_display context
 */
void draw_list_menu(MP3_DISPLAY *mp3);

/* draw space invader */
void space_invader(void);

/* space invader row */
void space_invader_row(void);