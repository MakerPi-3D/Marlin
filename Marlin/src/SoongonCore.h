/**
 *
 */
#pragma once

#include "inc/MarlinConfig.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if ENABLED(SOONGON_MINI_SECTION_CODE)

namespace sg_mini
{
  extern volatile float mini_z_height;
  extern volatile bool mini_is_endstops;
  extern volatile bool mini_is_eeprom_error;

  extern void mini_setup();

  extern void mini_load_unload_heatting_abort();

  extern bool mini_level_check_data(void);
  extern void mini_level_finish(void);
  extern bool mini_level_is_idle(void);
  extern bool mini_level_is_check(void);
  extern void mini_level_set_z_height(void);
  extern void mini_level_init(void);

  extern bool mini_key_print_find_0_gcode_file(const char *longFilename);

  extern void mini_set_heatting();

  extern void mini_led_set_warning();

  extern void mini_tick();

  extern void mini_set_homing(const bool is_done);

  extern void mini_heatting_finish();

#if ENABLED(SDSUPPORT)
  extern void mini_sd_abort_printing();
  extern void mini_sd_write_err_log(char *data);
#endif

  extern void mini_run();
}

#endif


class SoongonCore
{
public:
  static void run();
private:
#if ENABLED(SOONGON_I3_SECTION_CODE)
  static void i3_run();
#endif
};

extern SoongonCore sg_core;



