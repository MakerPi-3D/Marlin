
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../module/temperature.h"

namespace sg_mini
{

  static volatile bool mini_is_warning = false;

  void mini_led_init()
  {
    SET_OUTPUT(LED_RED_PIN);
    SET_OUTPUT(LED_GREEN_PIN);
    WRITE(LED_GREEN_PIN, true);
    WRITE(LED_RED_PIN, false);
  }

  void mini_led_process_for_print()
  {
    static bool turn_status = false;

    // Red light flashing during machine heating
    if (mini_key_print_is_heatting())
    {
      WRITE(LED_RED_PIN, turn_status);
      WRITE(LED_GREEN_PIN, false);
    }
    // The green light flashes in machine printing
    else if (mini_key_print_is_printting())
    {
      WRITE(LED_GREEN_PIN, turn_status);
      WRITE(LED_RED_PIN, false);
    }
    else if (mini_key_print_is_pausing())
    {
      WRITE(LED_GREEN_PIN, false);
      WRITE(LED_RED_PIN, true);
    }
    else if (mini_key_print_is_resuming())
    {
      WRITE(LED_GREEN_PIN, false);
      WRITE(LED_RED_PIN, turn_status);
    }

    turn_status = !turn_status;
  }

  void mini_led_process_for_load()
  {
    static bool turn_status = false;

    if (mini_key_load_is_prepare_heatting())
    {
      WRITE(LED_RED_PIN, turn_status);
      WRITE(LED_GREEN_PIN, false);
    }
    else if (mini_key_load_is_action())
    {
      WRITE(LED_RED_PIN, true);
      WRITE(LED_GREEN_PIN, false);
    }

    turn_status = !turn_status;
  }
  
  void mini_led_process_for_unload()
  {
    static bool turn_status = false;

    if (mini_key_unload_is_prepare_heatting())
    {
      WRITE(LED_RED_PIN, turn_status);
      WRITE(LED_GREEN_PIN, false);
    }
    else if (mini_key_unload_is_action())
    {
      WRITE(LED_RED_PIN, true);
      WRITE(LED_GREEN_PIN, false);
    }

    turn_status = !turn_status;
  }

  void mini_led_process()
  {
    if (mini_is_warning)
      return;

    if (mini_is_print)
    {
      mini_led_process_for_print();
    }
    else if (mini_is_load)
    {
      mini_led_process_for_load();
    }
    else if (mini_is_unload)
    {
      mini_led_process_for_unload();
    }
    else   // The green light is always on when the machine is idle
    {
      if (thermalManager.degHotend(0) < -10)
      {
        WRITE(LED_RED_PIN, true);
        WRITE(LED_GREEN_PIN, false);
      }
      else
      {
        WRITE(LED_GREEN_PIN, true);
        WRITE(LED_RED_PIN, false);
      }
    }
  }

  void mini_led_set_warning()
  {
    mini_is_warning = true;
  }

  void mini_led_warning_process()
  {
    static bool turn_status = false;
    static uint8_t warn_cnt = 0;

    if (!mini_is_warning)
      return;

    if (warn_cnt >= 5)
    {
      mini_is_warning = false;
      warn_cnt = 0;
      WRITE(LED_RED_PIN, false);
      WRITE(LED_GREEN_PIN, true);
    }

    if (turn_status)
    {
      turn_status = false;
      WRITE(LED_RED_PIN, false);
      WRITE(LED_GREEN_PIN, false);
    }
    else
    {
      turn_status = true;
      WRITE(LED_RED_PIN, true);
      WRITE(LED_GREEN_PIN, false);
      warn_cnt ++;
    }
  }


}

#endif


