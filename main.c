#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <hagl_hal.h>
#include <hagl.h>

#include <font6x9.h>

#include <wchar.h>

#include <mod_play.h>
#include <sound_i2s.h>

#include "hymn_to_aurora.h"

#include "graphics.h"

struct player {
    float x, y;
    int direction;
    int steps;
};

struct player player = {0, 0, 2, 0};

bool textBoxActive = false;

bool gp12justPressed = false;

wchar_t textLine1[26] = L"";
wchar_t textLine2[26] = L"";
wchar_t textLine3[26] = L"";
wchar_t textLine4[26] = L"";

static const struct sound_i2s_config sound_config = {
  .pin_scl         = 10,
  .pin_sda         = 9,
  .pin_ws          = 11,
  .sample_rate     = 22050,
  .bits_per_sample = 16,
  .pio_num         = 0,
};

void initInput() {
    gpio_init(5);
    gpio_set_dir(5, GPIO_IN);
    gpio_pull_up(5);

    gpio_init(6);
    gpio_set_dir(6, GPIO_IN);
    gpio_pull_up(6);

    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);

    gpio_init(8);
    gpio_set_dir(8, GPIO_IN);
    gpio_pull_up(8);

    gpio_init(12);
    gpio_set_dir(12, GPIO_IN);
    gpio_pull_up(12);

    gpio_init(13);
    gpio_set_dir(13, GPIO_IN);
    gpio_pull_up(13);

    gpio_init(14);
    gpio_set_dir(14, GPIO_IN);
    gpio_pull_up(14);

    gpio_init(15);
    gpio_set_dir(15, GPIO_IN);
    gpio_pull_up(15);
}

void handleInput() {
    if (!textBoxActive) {
        if (!gpio_get(5)) { // W
            player.y -= 0.7;
            player.direction = 0;
        }
        if (!gpio_get(6)) { // A
            player.x -= 0.7;
            player.direction = 1;
        }
        if (!gpio_get(7)) { // S
            player.y += 0.7;
            player.direction = 2;
        }
        if (!gpio_get(8)) { // D
            player.x += 0.7;
            player.direction = 3;
        }
    }
    if (gpio_get(5) && gpio_get(6) && gpio_get(7) && gpio_get(8)) {
        player.steps = 0;
    } else {
        player.steps++;
    }
    if (!gpio_get(12) && !gp12justPressed) { // I
        textBoxActive = !textBoxActive;
        gp12justPressed = true;
        wcscpy(textLine1, L"Hello");
        wcscpy(textLine2, L"World");
        wcscpy(textLine3, L"!");
        wcscpy(textLine4, L"abcdefghijklmnopqrstuvwxy");
    }
    if (gpio_get(12)) {
        gp12justPressed = false;
    }
    if (!gpio_get(13)) { // J
        wcscpy(textLine1, L"This");
        wcscpy(textLine2, L"Is");
        wcscpy(textLine3, L"TextBox");
        wcscpy(textLine4, L"2");
    }
    if (!gpio_get(14)) { // K
        
    }
    if (!gpio_get(15)) { // L
        
    }
}

static void update_mod_player(void)
{
  static int8_t *last_buffer;
  static unsigned char tmp_buffer[SOUND_I2S_BUFFER_NUM_SAMPLES];

  int8_t *buffer = sound_i2s_get_next_buffer();
  if (buffer != last_buffer) {
    last_buffer = buffer;

    // send MOD player output to tmp_buffer:
    mod_play_step(tmp_buffer, SOUND_I2S_BUFFER_NUM_SAMPLES);

    // copy tmp_buffer to I2S output buffer:
    for (int i = 0; i < SOUND_I2S_BUFFER_NUM_SAMPLES; i++) {
      int8_t sample = tmp_buffer[i] - 128;  // make it a signed 8-bit sample
      *buffer++ = sample;
      *buffer++ = sample;
      *buffer++ = sample;
      *buffer++ = sample;
    }
  }
}

void renderCharacter(hagl_backend_t *display, int px, int py, hagl_bitmap_t *bitmap) {
    hagl_color_t transparentColor = hagl_color(display, 0, 255, 0);
    for (uint8_t x = 0; x < 20; x++) {
        for (uint8_t y = 0; y < 30; y++) {
            hagl_color_t *color = (hagl_color_t *) (bitmap->buffer + (x + y * bitmap->width) * 2);
            if (*color != transparentColor) {
                hagl_put_pixel(display, px + x, py + y, *color);
            }
        }
    }
}

void gameLoop(hagl_backend_t *display) {
    while (1) {
        hagl_clear(display);
        handleInput();

        uint16_t w = 20;
        uint16_t h = 30;
        hagl_color_t color = 0xffff;
        hagl_blit(display, 0, 0, &wallTile);
        if (player.direction == 0) {
            if (player.steps > 60) {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkW3);
                if (player.steps > 80) {
                    player.steps = 0;
                }
            } else if (player.steps > 20 && player.steps < 40) {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkW2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkW1);
            }
        } else if (player.direction == 1) {
            if (player.steps > 20) {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkA2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkA1);
            }
        } else if (player.direction == 2) {
            if (player.steps > 60) {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkS3);
                if (player.steps > 80) {
                    player.steps = 0;
                }
            } else if (player.steps > 20 && player.steps < 40) {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkS2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                hagl_blit(display, (int)player.x, (int)player.y, &playerWalkS1);
            }
        } else if (player.direction == 3) {
            if (player.steps > 20) {
                // hagl_blit(display, (int)player.x, (int)player.y, &playerWalkD2);
                renderCharacter(display, (int)player.x, (int)player.y, &playerWalkD2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                // hagl_blit(display, (int)player.x, (int)player.y, &playerWalkD1);
                renderCharacter(display, (int)player.x, (int)player.y, &playerWalkD1);
            }
        }
        if (textBoxActive) {
            player.steps = 0;
            hagl_fill_rounded_rectangle_xyxy(display, 2, 82, 157, 125, 5, 0x0000);
            hagl_draw_rounded_rectangle_xyxy(display, 2, 82, 157, 125, 5, color);
            hagl_put_text(display, textLine1, 5, 85, color, font6x9);
            hagl_put_text(display, textLine2, 5, 95, color, font6x9);
            hagl_put_text(display, textLine3, 5, 105, color, font6x9);
            hagl_put_text(display, textLine4, 5, 115, color, font6x9);
        }
        update_mod_player();
        hagl_flush(display);
    }
}

int titleScreen(hagl_backend_t *display) {
    hagl_clear(display);
    hagl_put_text(display, L"PicoGame RPG (wip)", 26, 30, 0xffff, font6x9);
    hagl_put_text(display, L"Press L to start w/ music", 5, 50, 0xffff, font6x9);
    hagl_put_text(display, L"Press K to start no music", 5, 60, 0xffff, font6x9);

    hagl_blit(display, 13, 25, &logoText);
    hagl_flush(display);
    while (1) {
        if (!gpio_get(14)) return 0;
        if (!gpio_get(15)) return 1;
    }

}

int main()
{
    stdio_init_all();

    hagl_backend_t *display = hagl_init();

    initInput();

    sound_i2s_init(&sound_config);

    int soundActive = titleScreen(display);

    if (soundActive) {
        mod_play_start(&mod_hymn_to_aurora, 22050, 1);
        sound_i2s_playback_start();
    }

    gameLoop(display);

    hagl_close(display);

}