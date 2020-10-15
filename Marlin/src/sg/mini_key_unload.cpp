
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../gcode/queue.h"

#include "../module/temperature.h"
#include "../module/printcounter.h"
#include "../module/planner.h"
#include "../module/stepper.h"

namespace sg_mini
{

#define MINI_UNLOAD_PREPARE 1
#define MINI_UNLOAD_PREPARE_DONE 2
#define MINI_UNLOAD_HEATTING 3
#define MINI_UNLOAD_ACTION 4
#define MINI_UNLOAD_IDLE 0

  static void unload_key_function(uint8_t press_mode);

  MINI_KEY_T unload_key = {0, 0,  0,  0,  0, BTN_UNLOAD, unload_key_function };
  static volatile uint8_t mini_unload_status = MINI_UNLOAD_IDLE;
  static volatile millis_t unload_check_blocks_time = 0;

  static void unload_key_function(uint8_t press_mode)
  {
    switch (press_mode)
    {
    case KEY_SHORT_PRESS:
      break;

    case KEY_LONG_PRESS:
      mini_unload_status = MINI_UNLOAD_PREPARE;
      mini_is_unload = true;
      break;

    default:
      break;
    }
  }

  static void mini_key_unload_reset_status(void)
  {
    mini_unload_status = MINI_UNLOAD_IDLE;
    mini_is_unload = false;
  }

  void mini_key_unload_init(void)
  {
    SET_INPUT(BTN_UNLOAD);
    //filament startup not run
    unload_key.KeyProcessed = 1;
  }

  void mini_key_unload_heatting_done(void)
  {
    if (mini_is_unload && mini_unload_status == MINI_UNLOAD_HEATTING)
    {
      unload_check_blocks_time = millis() + 5000;
      queue.enqueue_now_P(PSTR("G92 E0"));
      queue.enqueue_now_P(PSTR("G1 F200 E10"));
      queue.enqueue_now_P(PSTR("G1 F200 E-400"));
      mini_unload_status = MINI_UNLOAD_ACTION;
    }
  }

  bool mini_key_unload_is_prepare_heatting(void)
  {
    return mini_unload_status == MINI_UNLOAD_PREPARE || mini_unload_status == MINI_UNLOAD_HEATTING;
  }

  bool mini_key_unload_is_action(void)
  {
    return mini_unload_status == MINI_UNLOAD_ACTION;
  }

  void mini_key_unload_heatting_abort()
  {
    if (mini_is_unload && mini_unload_status == MINI_UNLOAD_HEATTING)
    {
      if (digitalRead(unload_key.KeyPin))
      {
        print_job_timer.stop();
        thermalManager.setTargetHotend(0, 0);
        mini_key_unload_reset_status();
      }
    }
  }

  void mini_key_unload_check()
  {
    mini_key_check(unload_key);
  }

  void mini_key_unload_process()
  {
    if (mini_unload_status == MINI_UNLOAD_PREPARE)
    {
      if (!mini_is_homing)
      {
        queue.enqueue_now_P(PSTR("G28"));
      }

      queue.enqueue_now_P(PSTR("G1 F200 Z40"));
      mini_unload_status = MINI_UNLOAD_PREPARE_DONE;
    }
    else if (mini_unload_status == MINI_UNLOAD_PREPARE_DONE && !planner.has_blocks_queued())
    {
      queue.enqueue_now_P(PSTR("M109 S220"));
      mini_unload_status = MINI_UNLOAD_HEATTING;
    }
    else if (mini_unload_status == MINI_UNLOAD_ACTION)
    {
      if (digitalRead(unload_key.KeyPin))
      {
        stepper.quick_stop();
        queue.clear();
        queue.enqueue_now_P(PSTR("G92 E0"));
        thermalManager.setTargetHotend(0, 0);
        mini_key_unload_reset_status();
      }
      else if (!planner.has_blocks_queued() && ELAPSED(millis(), unload_check_blocks_time))
      {
        thermalManager.setTargetHotend(0, 0);
        mini_key_unload_reset_status();
      }
    }
  }
}

#endif


