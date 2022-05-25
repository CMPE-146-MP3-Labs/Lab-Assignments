#include "LCD_draw.h"
#include "fonts.h"
#include "song_list.h"
#include "ssd1309.h"
#include <stdio.h>
#include <string.h>

void draw_volume_on(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(volume_icon, sizeof(volume_icon));
}

void draw_volume_mute(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(volume_mute, sizeof(volume_mute));
}

void draw_play_pause(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(play_pause_icon, sizeof(play_pause_icon));
}

void draw_next_skip(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(next_skip_icon, sizeof(next_skip_icon));
}

void draw_previous_rewind(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(previous_rewind_icon, sizeof(previous_rewind_icon));
}

void draw_settings(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(settings_icon, sizeof(settings_icon));
}

void draw_columns(uint16_t number, uint8_t type) {
  LCD_Data_Mode();
  for (int i = 0; i < number; i++) {
    LCD_Write(type);
  }
}

void draw_volume(uint8_t current_volume) {
  if (current_volume == 0) {
    draw_volume_mute();
  } else if ((current_volume < 7) & (current_volume > 0)) {
    draw_volume_on();
    LCD_Set_Starting_RAM_Access_Pointer(PAGE7, 20);
    for (int i = 0; i < current_volume; i++) {
      draw_columns(14, 0x18);
      draw_columns(4, OFF);
    }
  } else {
    draw_volume_on();
  }
}

void draw_hamburger(void) { draw_columns(10, 0x92); }

void draw_return_icon(void) {
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 4);
  LCD_Data_Mode();
  ssp2__dma_write_block(return_icon[0], 10);
  LCD_Set_Starting_RAM_Access_Pointer(PAGE1, 8);
  LCD_Data_Mode();
  ssp2__dma_write_block(return_icon[1], 6);
}

void LCD_Write_String(const char *char_string) {
  int i = 0;
  bool mp3_part;
  while (char_string[i] != '\0') {

    mp3_part = (char_string[i] == '.') & (char_string[i + 1] == 'm') &
               (char_string[i + 2] == 'p') & (char_string[i + 3] == '3');
    if (mp3_part) {
      i += 4;
    } else {
      if (char_string[i] == ' ') {
        draw_columns(6, OFF);
      } else {
        /* Look up table */
        int j = 0;
        while (char_string[i] != lookup[j]) {
          j++;
        }
        ssp2__dma_write_block(character[j], 6);
      }
      i++;
    }
  }
}

void draw_up_icon(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(up_icon, sizeof(up_icon));
}

void draw_down_icon(void) {
  LCD_Data_Mode();
  ssp2__dma_write_block(down_icon, sizeof(down_icon));
}

static void verify_volume(MP3_DISPLAY *mp3) {
  if (mp3->current_volume > 6) {
    mp3->current_volume = 6;
  }
}

void draw_main_menu(MP3_DISPLAY *mp3) {
  LCD_CS();
  LCD_clear();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE5, 0);
  space_invader_row();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE2, 16);
  LCD_Write_String("NOW PLAYING:");
  LCD_Set_Starting_RAM_Access_Pointer(PAGE3, 16);
  LCD_Write_String(mp3->current_song);
  LCD_Set_Starting_RAM_Access_Pointer(PAGE7, 4);
  verify_volume(mp3);
  draw_volume(mp3->current_volume);
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 4);
  draw_hamburger();
  draw_columns(24, OFF);
  draw_previous_rewind();
  draw_columns(16, OFF);
  draw_play_pause();
  draw_columns(16, OFF);
  draw_next_skip();
  draw_columns(20, OFF);
  draw_settings();
  LCD_Cont_Horiz_Scroll_Setup(LEFT, PAGE3, 4, PAGE5, COL_MIN, COL_MAX);
  LCD_DS();
}

void draw_settings_menu(MP3_DISPLAY *mp3) {
  LCD_CS();
  char str[8];
  LCD_clear();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE7, 34);
  LCD_Write_String("-SETTINGS-");
  LCD_Set_Starting_RAM_Access_Pointer(PAGE5, 30);
  LCD_Write_String("CPU LOAD:");
  sprintf(str, "%d%%", mp3->cpu_load);
  LCD_Set_Starting_RAM_Access_Pointer(PAGE5, 84);
  LCD_Write_String(str);
  draw_return_icon();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE2, 30);
  LCD_Write_String("BASS:");
  if (mp3->current_bass < 10) {
    ssp2__dma_write_block(character[mp3->current_bass], 6);
  } else {
    LCD_Write_String("1");
    ssp2__dma_write_block(character[(mp3->current_bass) - 10], 6);
  }
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 30);
  LCD_Write_String("-    +");
  LCD_Set_Starting_RAM_Access_Pointer(PAGE2, 84);
  LCD_Write_String("TREB:");
  if (mp3->current_trebble < 10) {
    ssp2__dma_write_block(character[mp3->current_trebble], 6);
  } else {
    LCD_Write_String("1");
    ssp2__dma_write_block(character[(mp3->current_trebble) - 10], 6);
  }
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 84);
  LCD_Write_String("-    +");
  LCD_DS();
}

// todo: finish
void draw_list_menu(MP3_DISPLAY *mp3) {
  LCD_CS();
  LCD_clear();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE7, 34);
  LCD_Write_String("-SONG LIST-");
  draw_return_icon();
  /** TODO: write actual song list : DONE I THINK, WORKS SO FAR**/
  for (int i = 0; i < 4; i++) {
    LCD_Set_Starting_RAM_Access_Pointer((PAGE2 + i), 12);
    LCD_Write_String(
        song_list__get_name_for_item((mp3->current_song_page * 3) + i));
  }
  LCD_Set_Starting_RAM_Access_Pointer((PAGE2 + mp3->current_selection), 4);
  LCD_Write_String(">");
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 30);
  draw_up_icon();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 48);
  draw_down_icon();
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 72);
  LCD_Write_String("SEL");
  LCD_Set_Starting_RAM_Access_Pointer(PAGE0, 100);
  LCD_Write_String("NEXT");
  LCD_Cont_Horiz_Scroll_Setup(LEFT, (PAGE2 + mp3->current_selection), 6,
                              (PAGE2 + mp3->current_selection), 10, COL_MAX);
  LCD_DS();
}

void space_invader(void) {
  LCD_Data_Mode();
  LCD_Write(0x70);
  LCD_Write(0x18);
  LCD_Write(0x7D);
  LCD_Write(0xB6);
  LCD_Write(0xBC);
  LCD_Write(0x3C);
  LCD_Write(0xBC);
  LCD_Write(0xB6);
  LCD_Write(0x7D);
  LCD_Write(0x18);
  LCD_Write(0x70);
}

void space_invader_row(void) {
  space_invader();
  draw_columns(8, OFF);
  space_invader();
  draw_columns(8, OFF);
  space_invader();
  draw_columns(8, OFF);
  space_invader();
  draw_columns(8, OFF);
  space_invader();
  draw_columns(8, OFF);
  space_invader();
  draw_columns(8, OFF);
  space_invader();
}