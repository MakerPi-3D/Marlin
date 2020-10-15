
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

namespace sg_mini
{
  volatile bool mini_is_homing = false;
  volatile bool mini_is_endstops = false;
  volatile bool mini_is_print = false;
  volatile bool mini_is_load = false;
  volatile bool mini_is_unload = false;
  volatile bool mini_is_level = false;
  void mini_setup()
  {
    nvic_irq_set_priority(NVIC_SDIO, 2);
    nvic_irq_set_priority(NVIC_USART1, 2);
    safe_delay(50);
    mini_key_init();
    mini_led_init();
  }

  void mini_load_unload_heatting_abort()
  {
    mini_key_load_heatting_abort();
    mini_key_unload_heatting_abort();
  }

  void mini_set_heatting()
  {
    mini_key_print_set_heatting();
  }

  void mini_tick(void)
  {
    static uint16_t time_50_cnt, time_500_cnt;

    if (time_50_cnt > 50)
    {
      time_50_cnt = 0;
      mini_key_process();
      mini_led_warning_process();
    }
    else
    {
      time_50_cnt ++;
    }

    if (time_500_cnt > 500)
    {
      time_500_cnt = 0;
      mini_led_process();
    }
    else
    {
      time_500_cnt ++;
    }
  }

  void mini_set_homing(const bool is_done)
  {
    mini_is_homing = is_done;
  }

  void mini_heatting_finish()
  {
    mini_key_print_heatting_done();
    mini_key_load_heatting_done();
    mini_key_unload_heatting_done();
  }

  void mini_run()
  {
#if ENABLED(SDSUPPORT)
    mini_sd_process();
#endif
    mini_fan_process();
    mini_key_print_process();
    mini_key_load_process();
    mini_key_unload_process();
    mini_level_process();
  }
}

#endif


