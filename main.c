#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <hagl_hal.h>
#include <hagl.h>

#include <font6x9.h>



int main()
{
    stdio_init_all();

    hagl_backend_t *display = hagl_init();


    float x0 = 10;
    float y0 = 10;
    uint16_t w = 20;
    uint16_t h = 30;
    hagl_color_t color = 0xffff;

    bool onGround = false;

    float jumpVelocity = 0;

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

    /* Main loop. */
    while (1) {
        hagl_clear(display);
        if (!gpio_get(5)) {
            if (onGround) {
                jumpVelocity = -5;
                onGround = false;
            }
        }
        if (!gpio_get(6)) {
            x0 -= 0.66;
        }
        if (!gpio_get(7)) {
            y0 += 0.66;
        }
        if (!gpio_get(8)) {
            x0 += 0.66;
        }

        y0 += 0.5;
        y0 += jumpVelocity;
        jumpVelocity += 0.25;
        

        if (y0 > display->height - h) {
            y0 = display->height - h;
            onGround = true;
        }

        if (jumpVelocity > 0) jumpVelocity = 0;

        hagl_fill_rectangle_xywh(display, (int)x0, (int)y0, w, h, color);
        hagl_flush(display);
    };

hagl_close(display);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}