#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <hagl_hal.h>
#include <hagl.h>

#include <font6x9.h>

#include <wchar.h>

#include <mod_play.h>
#include <sound_i2s.h>

#include "flash_utils.h"

#include "hardware/flash.h" // for the flash erasing and writing
#include "hardware/sync.h" // for the interrupts
#include <string.h>

#include "hymn_to_aurora.h"

#include "graphics.h"
#include "interactableMap.h"

#define FLASH_TARGET_OFFSET (2044 * 1024)

struct player {
    float x, y;
    int direction;
    int steps;
};

struct player player = {30, 30, 2, 0};

bool textBoxActive = false;

bool gp15justPressed = false;

wchar_t textLine1[26] = L"";
wchar_t textLine2[26] = L"";
wchar_t textLine3[26] = L"";
wchar_t textLine4[26] = L"";

struct map (*currentMap) = &houseMap;

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

void renderSprite(hagl_backend_t *display, int px, int py, hagl_bitmap_t *bitmap) {
    hagl_color_t transparentColor = hagl_color(display, 0, 255, 0);
    for (uint8_t x = 0; x < bitmap->width; x++) {
        for (uint8_t y = 0; y < bitmap->height; y++) {
            hagl_color_t *color = (hagl_color_t *) (bitmap->buffer + (x + y * bitmap->width) * 2);
            if (*color != transparentColor) {
                hagl_put_pixel(display, px + x, py + y, *color);
            }
        }
    }
}

void renderMap(hagl_backend_t *display) {
    for (uint8_t x = 0; x < 10; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            switch (currentMap->map[y][x]) {
                case 'W':
                    hagl_blit(display, x*16, y*16, &wallTile);
                    break;
                case 'F':
                    hagl_blit(display, x*16, y*16, &floorTile);
                    break;
                case 'G':
                    hagl_blit(display, x*16, y*16, &grassTile);
                    break;
                case 'E':
                    hagl_blit(display, x*16, y*16, &fenceTile);
                    break;
                case 'R':
                    hagl_blit(display, x*16, y*16, &roadTile);
                    break;
                case 'C':
                    hagl_blit(display, x*16, y*16, &roadCenterTile);
                    break;
            }
        }
    }
}

void renderInteractableObjects(hagl_backend_t *display) {
    for (int i = 0; i < currentMap->numObjects; i++) {
        renderSprite(display, currentMap->objects[i]->x, currentMap->objects[i]->y, currentMap->objects[i]->sprite);
    }
}

void interactObject(hagl_backend_t *display) {
    int playerBoxX1 = player.x;
    int playerBoxX2 = player.x;
    int playerBoxY1 = player.y;
    int playerBoxY2 = player.y;
    switch (player.direction) {
        case 0:
            playerBoxY1 -= 30;
            playerBoxX1 -= 5;
            playerBoxX2 += 25;
            break;
        case 1:
            playerBoxX1 -= 30;
            playerBoxY1 += 5;
            playerBoxY2 += 35;
            break;
        case 2:
            playerBoxY1 += 30;
            playerBoxY2 += 60;
            playerBoxX2 += 25;
            playerBoxX1 -= 5;
            break;
        case 3:
            playerBoxX1 += 20;
            playerBoxX2 += 50;
            playerBoxY1 += 5;
            playerBoxY2 += 35;
            break;
    }
    for (int i = 0; i < currentMap->numObjects; i++) {
        if (
            playerBoxX1 < currentMap->objects[i]->x + currentMap->objects[i]->sprite->width &&
            playerBoxX2 > currentMap->objects[i]->x &&
            playerBoxY1 < currentMap->objects[i]->y + currentMap->objects[i]->sprite->height &&
            playerBoxY2 > currentMap->objects[i]->y
        ) {
            textBoxActive = true;
            // TODO: This code sucks, it is atrociously bad. If you are reading this code right now and know how to make it better, please submit a PR.
            int messageLength = wcslen(currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber]);
            if (messageLength < 26) {
                wcscpy(textLine1, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber]);
                wcscpy(textLine2, L"                         ");
                wcscpy(textLine3, L"                         ");
                wcscpy(textLine4, L"                         ");
            } else if (messageLength < 52) {
                wcscpy(textLine1, wcsncpy(textLine1, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber], 25));
                if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][25] == ' ') {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 26, 25));
                } else {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 25, 25));
                }
                wcscpy(textLine3, L"                         ");
                wcscpy(textLine4, L"                         ");
            } else if (messageLength < 78) {
                wcscpy(textLine1, wcsncpy(textLine1, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber], 25));
                if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][25] == ' ') {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 26, 25));
                    if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][51] == ' ') {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 52, 25));
                    } else {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 51, 25));
                    }
                } else {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 25, 25));
                    if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][50] == ' ') {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 51, 25));
                    } else {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 50, 25));
                    }
                }
                wcscpy(textLine4, L"                         ");
            } else {
                wcscpy(textLine1, wcsncpy(textLine1, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber], 25));
                if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][25] == ' ') {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 26, 25));
                    if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][51] == ' ') {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 52, 25));
                        if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][77] == ' ') {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 78, 25));
                        } else {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 77, 25));
                        }
                    } else {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 51, 25));
                        if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][76] == ' ') {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 77, 25));
                        } else {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 76, 25));
                        }
                    }
                } else {
                    wcscpy(textLine2, wcsncpy(textLine2, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 25, 25));
                    if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][50] == ' ') {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 51, 25));
                        if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][76] == ' ') {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 77, 25));
                        } else {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 76, 25));
                        }
                    } else {
                        wcscpy(textLine3, wcsncpy(textLine3, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 50, 25));
                        if (currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber][75] == ' ') {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 76, 25));
                        } else {
                            wcscpy(textLine4, wcsncpy(textLine4, currentMap->objects[i]->messages[currentMap->objects[i]->messageNumber] + 75, 25));
                        }
                    }
                }
            }

            if (currentMap->objects[i]->messageNumber < currentMap->objects[i]->numMessages - 1) {
                currentMap->objects[i]->messageNumber++;
            } else {
                continue;
            }
        }
    }
}

void menu(hagl_backend_t *display) {
    int selection = 0;
    int butPressed = 0;
    hagl_fill_rounded_rectangle_xyxy(display, 2, 2, 80, 125, 5, 0x0000);
    hagl_draw_rounded_rectangle_xyxy(display, 2, 2, 80, 125, 5, 0xffff);
    hagl_put_text(display, L"Menu", 5, 5, 0xffff, font6x9);
    hagl_put_text(display, L"Save", 15, 15, 0xffff, font6x9);
    hagl_put_text(display, L"Load", 15, 25, 0xffff, font6x9);
    hagl_put_text(display, L"Exit", 15, 35, 0xffff, font6x9);

    while (1) {
        hagl_fill_rectangle_xyxy(display, 7, 15, 10, 120, 0x0000);
        hagl_blit_xy(display, 7, 17 + selection * 10, &selector);

        hagl_flush(display);

        if (!gpio_get(5) && !butPressed) {
            selection--;
            if (selection < 0) {
                selection = 2;
            }
        }
        if (!gpio_get(7) && !butPressed) {
            selection++;
            if (selection > 2) {
                selection = 0;
            }
        }

        if (!gpio_get(5) || !gpio_get(7)) {
            butPressed = 1;
        } else {
            butPressed = 0;
        }


        if (!gpio_get(15)) {
            switch (selection) {
                case 0:
                    printf("Starting save\n");
                    uint8_t* playerSaveDataBytes = (uint8_t*) &player;
                    uint8_t mapSaveDataBytes;
                    if (currentMap == &houseMap) {
                        mapSaveDataBytes = 0;
                    } else if (currentMap == &outsideMap) {
                        mapSaveDataBytes = 1;
                    } else if (currentMap == &roadMap) {
                        mapSaveDataBytes = 2;
                    }
                    uint8_t* saveDataBytes = (uint8_t*) malloc(sizeof(player) + sizeof(mapSaveDataBytes));

                    memcpy(saveDataBytes, playerSaveDataBytes, sizeof(player));
                    memcpy(saveDataBytes + sizeof(player), &mapSaveDataBytes, sizeof(mapSaveDataBytes));

                    int saveDataSize = sizeof(saveDataBytes);
                    printf("Save size: %d\n", saveDataSize);

                    int writeSize = (saveDataSize / FLASH_PAGE_SIZE) + 1;
                    int sectorCount = ((writeSize * FLASH_PAGE_SIZE) / FLASH_SECTOR_SIZE) + 1; 
                    // int WRITE_ADRESS = (int) getAddressPersistent();
                    printf("Save address: %d\n", FLASH_TARGET_OFFSET);
                    printf("Save size: %d", FLASH_TARGET_OFFSET);

                    uint32_t interrupts = save_and_disable_interrupts();
                    flash_range_erase(FLASH_TARGET_OFFSET, sectorCount * FLASH_SECTOR_SIZE);
                    flash_range_program(FLASH_TARGET_OFFSET, saveDataBytes, FLASH_PAGE_SIZE * writeSize);
                    restore_interrupts(interrupts);
                    printf("Saved\n");

                    free(saveDataBytes);
                    printf("Saved2\n");
                    while (!gpio_get(15)) {}
                    return;
                case 1:
                    const uint8_t* flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
                    memcpy(&player, flash_target_contents, sizeof(player));
                    memcpy(&mapSaveDataBytes, flash_target_contents + sizeof(player), sizeof(mapSaveDataBytes));
                    switch (mapSaveDataBytes) {
                        case 0:
                            currentMap = &houseMap;
                            break;
                        case 1:
                            currentMap = &outsideMap;
                            break;
                        case 2:
                            currentMap = &roadMap;
                            break;
                    }
                    printf("Loaded\n");
                    while (!gpio_get(15)) {}
                    return;
                case 2:
                    while (!gpio_get(15)) {}
                    return;
            }
        }
        if (!gpio_get(14)) {
            return;
        }
    }
}

void handleInput(hagl_backend_t *display) {
    if (!textBoxActive) {
        if (!gpio_get(5)) { // W
            player.y -= 0.9;
            player.direction = 0;
        }
        if (!gpio_get(6)) { // A
            player.x -= 0.9;
            player.direction = 1;
        }
        if (!gpio_get(7)) { // S
            player.y += 0.9;
            player.direction = 2;
        }
        if (!gpio_get(8)) { // D
            player.x += 0.9;
            player.direction = 3;
        }
    }
    if (gpio_get(5) && gpio_get(6) && gpio_get(7) && gpio_get(8)) {
        player.steps = 0;
    } else {
        player.steps++;
    }
    if (!gpio_get(12)) { // I
        menu(display);
    }
    if (!gpio_get(13)) { // J
        printf("Player X: %f, Player Y: %f\n", player.x, player.y);
        printf("Save size: %d, %d\n", sizeof(player), sizeof(currentMap));
        printf("Save address: %x\n", getAddressPersistent());
    }
    if (!gpio_get(14)) { // K
        textBoxActive = false;
    }
    if (!gpio_get(15) && gp15justPressed) { // L
        interactObject(display);
        gp15justPressed = false;
    }
    if (gpio_get(15)) {
        gp15justPressed = true;
    }
}


void gameLoop(hagl_backend_t *display) {
    while (1) {
        hagl_clear(display);
        renderMap(display);

        uint16_t w = 20;
        uint16_t h = 30;
        hagl_color_t color = 0xffff;

        renderInteractableObjects(display);

        handleInput(display);
        
        if (player.direction == 0) {
            if (player.steps > 60) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkW3);
                if (player.steps > 80) {
                    player.steps = 0;
                }
            } else if (player.steps > 20 && player.steps < 40) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkW2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkW1);
            }
        } else if (player.direction == 1) {
            if (player.steps > 20) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkA2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkA1);
            }
        } else if (player.direction == 2) {
            if (player.steps > 60) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkS3);
                if (player.steps > 80) {
                    player.steps = 0;
                }
            } else if (player.steps > 20 && player.steps < 40) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkS2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkS1);
            }
        } else if (player.direction == 3) {
            if (player.steps > 20) {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkD2);
                if (player.steps > 40) {
                    player.steps = 0;
                }
            } else {
                renderSprite(display, (int)player.x, (int)player.y, &playerWalkD1);
            }
        }

        if (player.x > 150 && player.y > 35 && player.y < 60 && currentMap == &houseMap) {
            currentMap = &outsideMap;
            player.x = 10;
        }
        if (player.x < 10 && player.y > 35 && player.y < 60 && currentMap == &outsideMap) {
            currentMap = &houseMap;
            player.x = 140;
        }
        if (player.x > 55 && player.x < 85 && player.y > 110 && currentMap == &outsideMap) {
            currentMap = &roadMap;
            player.y = 0;
        }
        if (player.x > 55 && player.x < 85 && player.y < -10 && currentMap == &roadMap) {
            currentMap = &outsideMap;
            player.y = 100;
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

    hagl_blit(display, 8, 20, &logoText);
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