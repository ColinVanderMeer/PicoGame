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

    /* Main loop. */
    while (1) {
        hagl_clear(display);
        for (uint16_t i = 1; i < 50; i++) {
            int16_t x0 = rand() % display->width;
            int16_t y0 = rand() % display->height;
            hagl_color_t color = rand() % 0xffff;

            hagl_put_text(display, u"Hello", x0, y0, color, font6x9);
        }
        hagl_flush(display);
    };

hagl_close(display);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}