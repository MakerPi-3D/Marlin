
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../module/temperature.h"
#include "../module/printcounter.h"
#include "../module/planner.h"

#include "../gcode/queue.h"

#if ENABLED(SDSUPPORT)
  #include "../sd/cardreader.h"
#endif

namespace sg_mini
{
#define MINI_PRINT_START_PRINT 1
#define MINI_PRINT_HEATTING 2
#define MINI_PRINT_PRINTTING 3
#define MINI_PRINT_PAUSING 4
#define MINI_PRINT_RESUMING 5
#define MINI_PRINT_IDLE 0

#define MINI_PAUSE_IDLE 0
#define MINI_PAUSE_START 1
#define MINI_PAUSE_WAITING 2
#define MINI_PAUSE_COOLING 3
#define MINI_PAUSE_DROPPING 4
#define MINI_PAUSE_DROPPING_WAITING 5

#define MINI_RESUME_IDLE 0
#define MINI_RESUME_START 1
#define MINI_RESUME_HEATING 2
#define MINI_RESUME_RISING 3
#define MINI_RESUME_START_PRINTING 4

  static void print_key_function(uint8_t press_mode);

  MINI_KEY_T print_key = {0, 0,  0,  0,  0, BTN_PRINT, print_key_function };
  static char *gcode_0_file_ptr;
  static volatile uint8_t mini_pause_status = MINI_PAUSE_IDLE;
  static volatile uint8_t mini_resume_status = MINI_RESUME_IDLE;
  static volatile uint8_t mini_print_status = MINI_PRINT_IDLE;
  static volatile float target_hotend_temp = 0.0f;

  static void mini_key_print_reset_status()
  {
    mini_print_status = MINI_PRINT_IDLE;
    mini_pause_status = MINI_PAUSE_IDLE;
    mini_resume_status = MINI_RESUME_IDLE;
    mini_is_print = false;
  }

  static void print_key_function(uint8_t press_mode)
  {
    switch (press_mode)
    {
    case KEY_SHORT_PRESS:
      if (mini_pause_status != MINI_PAUSE_IDLE || mini_resume_status != MINI_RESUME_IDLE)
        break;

      if (mini_print_status == MINI_PRINT_IDLE)
      {
        print_key.KeyAfter = 1;
        mini_print_status = MINI_PRINT_START_PRINT;
        SERIAL_ECHOLNPAIR("key==>>start print press");
      }
      else if (mini_print_status == MINI_PRINT_PAUSING)
      {
        mini_print_status = MINI_PRINT_RESUMING;
        mini_resume_status = MINI_RESUME_START;
        SERIAL_ECHOLNPAIR("key==>>resume press");
      }
      else if (mini_print_status == MINI_PRINT_PRINTTING)
      {
        mini_print_status = MINI_PRINT_PAUSING;
        mini_pause_status = MINI_PAUSE_START;
        SERIAL_ECHOLNPAIR("key==>>pause press");
      }

      break;

    case KEY_LONG_PRESS:
      thermalManager.setTargetHotend(0, 0);
      card.flag.abort_sd_printing = true;
      mini_key_print_reset_status();
      SERIAL_ECHOLNPAIR("key==>>abort print press");
      break;

    default:
      break;
    }
  }

#if ENABLED(SDSUPPORT)

  static void mini_key_print_for_print(void)
  {
    if (print_key.KeyAfter)
    {
      card.ls();

      if (gcode_0_file_ptr == NULL)
      {
        const uint16_t fileCnt = card.get_num_Files();
        card.getfilename_sorted(fileCnt);
      }

      if (card.filename == NULL)
      {
        mini_key_print_reset_status();
        SERIAL_ECHOLNPAIR("mini_key_print_for_print==>>open failed, File: file does not exist");
      }
      else
      {
        card.openAndPrintFile((char *)card.filename);
        SERIAL_ECHOLNPAIR("mini_key_print_for_print==>>open file: ", card.filename);
        mini_is_print = true;
        mini_print_status = MINI_PRINT_PRINTTING;
      }

      gcode_0_file_ptr = NULL;
      print_key.KeyAfter = 0;
    }
  }

  static void mini_key_print_for_pause(void)
  {
    if (mini_pause_status == MINI_PAUSE_START)
    {
      if (IS_SD_PRINTING()) card.pauseSDPrint();

      print_job_timer.pause();
      mini_pause_status = MINI_PAUSE_WAITING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_pause==>>pausing, please waiting!");
    }
    else if (mini_pause_status == MINI_PAUSE_WAITING && !planner.has_blocks_queued())
    {
      mini_pause_status = MINI_PAUSE_COOLING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_pause==>>pausing done");
    }
    else if (mini_pause_status == MINI_PAUSE_COOLING)
    {
      target_hotend_temp = thermalManager.degTargetHotend(0);
      thermalManager.setTargetHotend(target_hotend_temp / 2, 0);
      mini_pause_status = MINI_PAUSE_DROPPING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_pause==>>cooling to 60");
    }
    else if (mini_pause_status == MINI_PAUSE_DROPPING)
    {
      queue.enqueue_now_P(PSTR("G91"));
      queue.enqueue_now_P(PSTR("G0 F300 Z+20"));
      queue.enqueue_now_P(PSTR("G90"));
      mini_pause_status = MINI_PAUSE_DROPPING_WAITING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_pause==>>dropping 20mm");
    }
    else if (mini_pause_status == MINI_PAUSE_DROPPING_WAITING && !planner.has_blocks_queued())
    {
      mini_pause_status = MINI_PAUSE_IDLE;
      SERIAL_ECHOLNPAIR("mini_key_print_for_pause==>>dropping 20mm done");
    }
  }

  static void mini_key_print_for_resume(void)
  {
    if (mini_resume_status == MINI_RESUME_START)
    {
      thermalManager.setTargetHotend(target_hotend_temp, 0);
      mini_resume_status = MINI_RESUME_HEATING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_resume==>>heatting");
    }
    else if (mini_resume_status == MINI_RESUME_HEATING && !thermalManager.isHeatingHotend(0))
    {
      mini_resume_status = MINI_RESUME_RISING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_resume==>>heatting done");
    }
    else if (mini_resume_status == MINI_RESUME_RISING)
    {
      queue.enqueue_now_P(PSTR("G91"));
      queue.enqueue_now_P(PSTR("G0 F300 Z-20"));
      queue.enqueue_now_P(PSTR("G90"));
      mini_resume_status = MINI_RESUME_START_PRINTING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_resume==>>rising");
    }
    else if (mini_resume_status == MINI_RESUME_START_PRINTING && !planner.has_blocks_queued())
    {
      queue.enqueue_now_P(PSTR("M24"));
      mini_resume_status = MINI_RESUME_IDLE;
      mini_print_status = MINI_PRINT_PRINTTING;
      SERIAL_ECHOLNPAIR("mini_key_print_for_resume==>>rising done");
    }
  }

#endif

  void mini_key_print_init(void)
  {
    SET_INPUT(BTN_PRINT);
  }

  void mini_key_print_check()
  {
    mini_key_check(print_key);
  }

  bool mini_key_print_is_heatting(void)
  {
    return mini_print_status == MINI_PRINT_HEATTING;
  }

  bool mini_key_print_is_printting(void)
  {
    return mini_print_status == MINI_PRINT_PRINTTING;
  }

  bool mini_key_print_is_pausing(void)
  {
    return mini_print_status == MINI_PRINT_PAUSING;
  }

  bool mini_key_print_is_resuming(void)
  {
    return mini_print_status == MINI_PRINT_RESUMING;
  }

  void mini_key_print_heatting_done()
  {
    if (mini_is_print)
    {
      mini_print_status = MINI_PRINT_PRINTTING;
    }
  }

  void mini_key_print_set_heatting()
  {
    if (mini_is_print)
    {
      mini_print_status = MINI_PRINT_HEATTING;
    }
  }

  bool mini_key_print_find_0_gcode_file(const char *longFilename)
  {
    if (print_key.KeyAfter)
    {
      gcode_0_file_ptr = strstr((char *)longFilename, (char *)"0.gcode");

      if (gcode_0_file_ptr != NULL)
      {
        return true;
      }
    }

    return false;
  }

  void mini_key_print_process(void)
  {
#if ENABLED(SDSUPPORT)
    if (!card.isMounted()) return;

    if (mini_print_status == MINI_PRINT_START_PRINT)
    {
      mini_key_print_for_print();
    }
    else if (mini_print_status == MINI_PRINT_PAUSING)
    {
      mini_key_print_for_pause();
    }
    else if (mini_print_status == MINI_PRINT_RESUMING)
    {
      mini_key_print_for_resume();
    }

    if (marlin_state == MF_SD_COMPLETE)
    {
      mini_key_print_reset_status();
    }

#endif
  }
}

#endif


