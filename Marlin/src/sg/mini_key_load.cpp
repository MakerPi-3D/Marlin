
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../gcode/queue.h"

#include "../module/temperature.h"
#include "../module/printcounter.h"
#include "../module/planner.h"
#include "../module/stepper.h"

namespace sg_mini
{

#define MINI_LOAD_PREPARE 1
#define MINI_LOAD_PREPARE_DONE 2
#define MINI_LOAD_HEATTING 3
#define MINI_LOAD_ACTION 4
#define MINI_LOAD_IDLE 0

  static void load_key_function(uint8_t press_mode);

  MINI_KEY_T load_key = {0, 0,  0,  0,  0, BTN_LOAD, load_key_function };
  static volatile uint8_t mini_load_status = MINI_LOAD_IDLE;
  static volatile millis_t load_check_blocks_time = 0;

  static void load_key_function(uint8_t press_mode)
  {
    switch (press_mode)
    {
    case KEY_SHORT_PRESS:
      break;

    case KEY_LONG_PRESS:
      mini_load_status = MINI_LOAD_PREPARE;
      mini_is_load = true;
      break;

    default:
      break;
    }
  }

  static void mini_key_load_reset_status(void)
  {
    mini_load_status = MINI_LOAD_IDLE;
    mini_is_load = false;
  }

  void mini_key_load_init(void)
  {
    SET_INPUT(BTN_LOAD);
    //filament startup not run
    load_key.KeyProcessed = 1;
  }

  void mini_key_load_heatting_done(void)
  {
    if (mini_is_load && mini_load_status == MINI_LOAD_HEATTING)
    {
      load_check_blocks_time = millis() + 5000;
      queue.enqueue_now_P(PSTR("G92 E0"));
      queue.enqueue_now_P(PSTR("G1 F200 E400"));
      sg_mini::mini_load_status = MINI_LOAD_ACTION;
    }
  }

  bool mini_key_load_is_prepare_heatting(void)
  {
    return mini_load_status == MINI_LOAD_PREPARE || mini_load_status == MINI_LOAD_HEATTING;
  }

  bool mini_key_load_is_action(void)
  {
    return mini_load_status == MINI_LOAD_ACTION;
  }

  void mini_key_load_heatting_abort()
  {
    if (mini_is_load && mini_load_status == MINI_LOAD_HEATTING)
    {
      if (digitalRead(load_key.KeyPin))
      {
        print_job_timer.stop();
        thermalManager.setTargetHotend(0, 0);
        mini_key_load_reset_status();
      }
    }
  }

  void mini_key_load_check()
  {
    mini_key_check(load_key);
  }

  void mini_key_load_process()
  {
    if (mini_load_status == MINI_LOAD_PREPARE)
    {
      if (!mini_is_homing)
      {
        queue.enqueue_now_P(PSTR("G28"));
      }

      queue.enqueue_now_P(PSTR("G1 F200 Z40"));
      mini_load_status = MINI_LOAD_PREPARE_DONE;
    }
    else if (mini_load_status == MINI_LOAD_PREPARE_DONE && !planner.has_blocks_queued())
    {
      queue.enqueue_now_P(PSTR("M109 S220"));
      mini_load_status = MINI_LOAD_HEATTING;
    }
    else if (mini_load_status == MINI_LOAD_ACTION)
    {
      if (digitalRead(load_key.KeyPin))
      {
        stepper.quick_stop();
        queue.clear();
        queue.enqueue_now_P(PSTR("G92 E0"));
        thermalManager.setTargetHotend(0, 0);
        mini_key_load_reset_status();
      }
      else if (!planner.has_blocks_queued() && ELAPSED(millis(), load_check_blocks_time))
      {
        thermalManager.setTargetHotend(0, 0);
        mini_key_load_reset_status();
      }
    }
  }

}

#endif


