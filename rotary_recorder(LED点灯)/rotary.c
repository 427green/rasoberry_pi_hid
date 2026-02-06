#include "pico/stdlib.h"
#include <stdio.h>

\\ロータリエンコーダを右に回すとＲ(ight)LED 左に回すとL(eft)LEDが点灯


#define LED_R 3
#define LED_L 4
\\ロータリエンコーダーのGPIO
#define CLK 2
#define DT 1
#define SW 0

void Rotary_recorder() {
  bool last_clk = gpio_get(CLK);

  while (true) {
    if (!gpio_get(SW)) {
      gpio_put(LED_R, 0);
      gpio_put(LED_L, 0);
    }
    bool current_clk = gpio_get(CLK);
    if (current_clk != last_clk) {
      if (current_clk == 0) {
        if (gpio_get(DT) != current_clk) {
          gpio_put(LED_R, 1);
          gpio_put(LED_L, 0);
          sleep_ms(5);
          gpio_put(LED_R, 0);
        } else {
          gpio_put(LED_R, 0);
          gpio_put(LED_L, 1);
          sleep_ms(5);
          gpio_put(LED_L, 0);
        }
      }
      last_clk = current_clk;
    }

    sleep_ms(1);
  }
}

int main() {
  /////////////////////////////////////////////////////////////
  stdio_init_all();

  gpio_init(LED_R);
  gpio_init(LED_L);
  gpio_init(CLK);
  gpio_init(DT);
  gpio_init(SW);

  gpio_set_dir(LED_R, GPIO_OUT);
  gpio_set_dir(LED_L, GPIO_OUT);
  gpio_set_dir(CLK, GPIO_IN);
  gpio_set_dir(DT, GPIO_IN);
  gpio_set_dir(SW, GPIO_IN);

  gpio_pull_up(CLK);
  gpio_pull_up(DT);
  gpio_pull_up(SW);
  /////////////////////////////////////////////////////////////

  while (true) {
    Rotary_recorder();
  }
}
