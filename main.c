#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

int main()
{
    stdio_init_all();
    st7735_init();


    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
