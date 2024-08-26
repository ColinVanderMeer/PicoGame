#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <hagl_hal.h>
#include <hagl.h>

#include <font6x9.h>

#include <wchar.h>

struct player {
    float x, y;
    int direction;
};

struct player player = {0, 0, 0};

bool textBoxActive = false;

bool gp12justPressed = false;

wchar_t textLine1[26] = L"";
wchar_t textLine2[26] = L"";
wchar_t textLine3[26] = L"";
wchar_t textLine4[26] = L"";

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
            player.y -= 1;
        }
        if (!gpio_get(6)) { // A
            player.x -= 1;
        }
        if (!gpio_get(7)) { // S
            player.y += 1;
        }
        if (!gpio_get(8)) { // D
            player.x += 1;
        }
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

void gameLoop(hagl_backend_t *display) {
    while (1) {
        hagl_clear(display);
        handleInput();

        uint16_t w = 20;
        uint16_t h = 30;
        hagl_color_t color = 0xffff;

        hagl_fill_rectangle_xywh(display, (int)player.x, (int)player.y, w, h, color);
        if (textBoxActive) {
            hagl_fill_rounded_rectangle_xyxy(display, 2, 84, 157, 127, 5, 0x0000);
            hagl_draw_rounded_rectangle_xyxy(display, 2, 84, 157, 127, 5, color);
            hagl_put_text(display, textLine1, 5, 87, color, font6x9);
            hagl_put_text(display, textLine2, 5, 97, color, font6x9);
            hagl_put_text(display, textLine3, 5, 107, color, font6x9);
            hagl_put_text(display, textLine4, 5, 117, color, font6x9);
        }
        hagl_flush(display);
    }
}

int main()
{
    stdio_init_all();

    hagl_backend_t *display = hagl_init();

    initInput();

    gameLoop(display);

    hagl_close(display);

}