/**
 *
 */
#pragma once

#include <stdint.h>

#include "../inc/MarlinConfig.h"

#define DEBUG_OUT ENABLED(SOONGON_MINI_SECTION_CODE)
#include "../../core/debug_out.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#define KEY_SHORT_PRESS 0x00
#define KEY_LONG_PRESS  0x01

namespace sg_mini
{
  typedef struct
  {
    uint8_t KeyAge;
    uint8_t KeyPress;
    uint8_t KeyHold;
    uint8_t KeyProcessed ;
    uint8_t KeyAfter;
    pin_t KeyPin;
    void (*fun)(uint8_t);
  } MINI_KEY_T;

  extern volatile bool mini_is_homing;
  extern volatile bool mini_is_print;
  extern volatile bool mini_is_load;
  extern volatile bool mini_is_unload;
  extern volatile bool mini_is_level;
  extern volatile float mini_z_height;

  extern void mini_led_init();
  extern void mini_led_process();
  extern void mini_led_warning_process();

  extern void mini_key_check(MINI_KEY_T &key);
  extern void mini_key_init(void);
  extern void mini_key_process(void);

  extern void mini_key_print_init(void);
  extern void mini_key_print_process(void);
  extern void mini_key_print_check();
  extern void mini_key_print_heatting_done();
  extern void mini_key_print_set_heatting();
  extern bool mini_key_print_is_heatting(void);
  extern bool mini_key_print_is_printting(void);
  extern bool mini_key_print_is_pausing(void);
  extern bool mini_key_print_is_resuming(void);

  extern void mini_key_load_init(void);
  extern void mini_key_load_check(void);
  extern void mini_key_load_process(void);
  extern void mini_key_load_heatting_abort(void);
  extern void mini_key_load_heatting_done(void);
  extern bool mini_key_load_is_prepare_heatting(void);
  extern bool mini_key_load_is_action(void);
  
  extern void mini_key_unload_init(void);
  extern void mini_key_unload_check(void);
  extern void mini_key_unload_process(void);
  extern void mini_key_unload_heatting_abort(void);
  extern void mini_key_unload_heatting_done(void);
  extern bool mini_key_unload_is_prepare_heatting(void);
  extern bool mini_key_unload_is_action(void);

  extern void mini_fan_process();

  #if ENABLED(SDSUPPORT)
    extern void mini_sd_process();
  #endif
  
  extern bool mini_level_is_idle(void);
  extern void mini_level_process(void);
}


#endif



