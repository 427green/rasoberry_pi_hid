#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "tusb.h"
#include "usb_descriptors.h"


#define SW 0
#define CLK 2
#define DT 1


void my_hardware_init(void);
void hid_task(void);
void encoder_task(void);

void my_hardware_init(void) {
  stdio_init_all();


  gpio_init(SW);
  gpio_set_dir(SW, GPIO_IN);
  gpio_pull_up(SW);


  gpio_init(CLK);
  gpio_set_dir(CLK, GPIO_IN);
  gpio_pull_up(CLK);

  gpio_init(DT);
  gpio_set_dir(DT, GPIO_IN);
  gpio_pull_up(DT);
}

static int8_t encoder_delta = 0;

void encoder_task(void) {
  static uint8_t last_state = 0;

  // CLKとDTの状態を2ビットにまとめる
  uint8_t current_state = (gpio_get(CLK) << 1) | gpio_get(DT);

  if (current_state != last_state) {
    if ((last_state == 0x00 && current_state == 0x01) ||
        (last_state == 0x03 && current_state == 0x02)) {
      encoder_delta = 1;
    } else if ((last_state == 0x00 && current_state == 0x02) ||
               (last_state == 0x03 && current_state == 0x01)) {
      encoder_delta = -1; 
    }
    last_state = current_state;
  }
}

void hid_task(void) {
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  uint32_t current_time_ms = to_ms_since_boot(get_absolute_time());
  if (current_time_ms - start_ms < interval_ms)
    return;
  start_ms = current_time_ms;

  if (!tud_hid_ready())
    return;

  uint8_t keycode[6] = {0};
  bool report_needed = false;

  if (gpio_get(SW) == 0) {
    keycode[0] = HID_KEY_A;
    report_needed = true;
  }

  if (encoder_delta > 0) {
    keycode[1] = HID_KEY_ARROW_UP;
    encoder_delta = 0; 
    report_needed = true;
  } else if (encoder_delta < 0) {
    keycode[1] = HID_KEY_ARROW_DOWN;
    encoder_delta = 0; 
    report_needed = true;
  }


  static bool last_was_pressed = false;
  if (report_needed) {
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
    last_was_pressed = true;
  } else if (last_was_pressed) {
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    last_was_pressed = false;
  }
}

int main(void) {
  my_hardware_init();
  tusb_init();

  
  while (1) {
    tud_task();
    encoder_task(); 
    hid_task();   
  }
}

// TinyUSB callbacks
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}
