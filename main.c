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
    if (!gpio_get(5)) {
        player.y -= 1;
    }
    if (!gpio_get(6)) {
        player.x -= 1;
    }
    if (!gpio_get(7)) {
        player.y += 1;
    }
    if (!gpio_get(8)) {
        player.x += 1;
    }
    if (!gpio_get(12)) {
        
    }
    if (!gpio_get(13)) {
        
    }
    if (!gpio_get(14)) {
        
    }
    if (!gpio_get(15)) {
        
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

        hagl_put_text(display, L"This is a text test :)", 0, 100, color, font6x9);
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