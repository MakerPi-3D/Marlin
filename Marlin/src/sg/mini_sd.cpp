
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../gcode/queue.h"

#include "../module/stepper.h"

#if ENABLED(SDSUPPORT)
  #include "../sd/cardreader.h"
#endif

namespace sg_mini
{

#if ENABLED(SDSUPPORT)

#define MINI_SD_LOG_FILE             ((const char *)"ErrLog.txt")
#define MINI_SD_LOG_BUG_SIZE 128

  static void mini_sd_create_log_file(const char *const fname)
  {
    if (!card.isMounted()) return;

    static SdFile curDir = card.getroot();
    SdBaseFile file;

    if (file.open(&curDir, fname, O_CREAT | O_EXCL | O_RDWR))
    {
      file.close();
    }
    else
    {
      SERIAL_ECHOLNPGM("mini_sd_create_log_file==>>file exist!");
    }
  }

  void mini_sd_abort_printing()
  {
    if(stepper.get_position_length(Z_AXIS) <= Z_MAX_POS - 10)
    {
      queue.enqueue_now_P(PSTR("G91"));
      queue.enqueue_now_P(PSTR("G0 F300 Z+10"));
      queue.enqueue_now_P(PSTR("G90"));
    }

    if (!sg_mini::mini_is_homing)
      queue.enqueue_now_P(PSTR("G28 X0 Y0"));

    queue.enqueue_now_P(PSTR("G1 F1800 X0 Y100"));
  }

  void mini_sd_process()
  {
    static bool create_log_files_once = false;

    if (!create_log_files_once)
    {
      mini_sd_create_log_file(MINI_SD_LOG_FILE);
      create_log_files_once = true;
    }
  }

  void mini_sd_write_err_log(char *data)
  {
    if (!card.isMounted()) return;

    static char *data_bak = 0;
    static SdFile curDir = card.getroot();
    SdBaseFile file;
    char log_data[MINI_SD_LOG_BUG_SIZE] = {0};

    if (data_bak == data) return;

    data_bak = data;
    (void)snprintf(&log_data[0], MINI_SD_LOG_BUG_SIZE - 3, data);
    log_data[MINI_SD_LOG_BUG_SIZE - 2] = 0x0d;
    log_data[MINI_SD_LOG_BUG_SIZE - 1] = 0x0a;

    if (file.open(&curDir, MINI_SD_LOG_FILE, O_APPEND | O_WRITE))
    {
      file.write(&log_data[0], MINI_SD_LOG_BUG_SIZE);
      file.close();
      SERIAL_ECHOLNPAIR("mini_sd_write_err_log==>>", data);
    }
    else
      SERIAL_ECHOLNPGM("mini_sd_write_err_log==>>open failed!");
  }

#endif

}

#endif


