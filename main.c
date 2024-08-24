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


    int16_t x0 = 10;
    int16_t y0 = 10;
    uint16_t w = 100;
    uint16_t h = 100;
    hagl_color_t color = 0xffff;

    /* Main loop. */
    while (1) {
        hagl_clear(display);
        hagl_fill_rectangle_xywh(display, x0, y0, w, h, color);
        hagl_flush(display);
    };

hagl_close(display);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}