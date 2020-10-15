
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../gcode/queue.h"
#include "../gcode/parser.h"

#include "../module/stepper.h"

#if ENABLED(SDSUPPORT)
  #include "../sd/cardreader.h"
#endif

#if ENABLED(EEPROM_SETTINGS)
  #include "../module/settings.h"
#endif

#if HAS_LEVELING
  #include "../feature/bedlevel/bedlevel.h"
#endif

namespace sg_mini
{

#define MINI_LEVEL_IDLE 0
#define MINI_LEVEL_START 1
#define MINI_LEVEL_PROCESS 2
#define MINI_LEVEL_GET_Z_HEIGHT 3
#define MINI_LEVEL_CHECK 4

#define MINI_LEVEL_LOG_FILE              ((const char *)"LevelLog.txt")
#define MINI_LEVEL_SET                   ((char *)"SET:")
#define MINI_LEVEL_S00                   ((char *)"S00:")
#define MINI_LEVEL_S01                   ((char *)"S01:")
#define MINI_LEVEL_S02                   ((char *)"S02:")
#define MINI_LEVEL_S10                   ((char *)"S10:")
#define MINI_LEVEL_S11                   ((char *)"S11:")
#define MINI_LEVEL_S12                   ((char *)"S12:")
#define MINI_LEVEL_S20                   ((char *)"S20:")
#define MINI_LEVEL_S21                   ((char *)"S21:")
#define MINI_LEVEL_S22                   ((char *)"S22:")
#define MINI_LEVEL_Z_HEIGHT              ((char *)"Z_Height:")
#define MINI_LEVEL_LOG_BUG_SIZE          512

#define MINI_LEVEL_Z_HEIGHT_OFFSET 0.2f

  static volatile uint8_t mini_level_status = MINI_LEVEL_IDLE;
  static volatile char mini_level_set_mark = 0;
  float volatile mini_z_height = 0;
  volatile bool mini_is_eeprom_error = false;

  void mini_level_init(void)
  {
    SET_INPUT(LEVEL_START);

    if (digitalRead(LEVEL_START) == false)
    {
      SET_INPUT(LEVEL_CHECK);
      mini_level_status = MINI_LEVEL_START;
      mini_is_level = true;
    }
    else
    {
      MYSERIAL1.begin(BAUDRATE);
    }
  }

#if ENABLED(SDSUPPORT)
  static char *mini_sd_find_key_value(const char *key, char *data)
  {
    static char *value;
    value = strstr((char *)data, key);

    if (value != NULL)
      value = value + strlen(key);

    return value;
  }

  static void mini_level_read_set_data(char *str)
  {
    char *ptr = mini_sd_find_key_value(MINI_LEVEL_SET, str);

    if (ptr != NULL)
    {
      mini_level_set_mark = (float)atoi(ptr);
    }
    else
    {
      card.removeFile(MINI_LEVEL_LOG_FILE);
      SERIAL_ECHOLNPGM("mini_level_read_set_data==>>The data is incomplete.");
      return;
    }

    if (!mini_level_set_mark)
    {
      SERIAL_ECHOLNPGM("mini_level_read_set_data==>>No data needs to be set.");
      return;
    }
  }

  static void mini_level_read_level_data(char *str, float *data)
  {
    char *ptr = NULL;
    char *level_str[9] = {MINI_LEVEL_S00, MINI_LEVEL_S01, MINI_LEVEL_S02, MINI_LEVEL_S10, MINI_LEVEL_S11, MINI_LEVEL_S12, MINI_LEVEL_S20, MINI_LEVEL_S21, MINI_LEVEL_S22};

    for (int i = 0; i < 9; i++)
    {
      ptr = mini_sd_find_key_value(level_str[i], str);

      if (ptr != NULL)
      {
        data[i] = (float)atof(ptr);
        ptr = NULL;
      }
      else
      {
        SERIAL_ECHOLNPGM("mini_level_read_level_data==>>The data is incomplete.");
        return;
      }
    }
  }

  static void mini_level_read_z_height_data(char *str)
  {
    char *ptr = mini_sd_find_key_value(MINI_LEVEL_Z_HEIGHT, str);

    if (ptr != NULL)
    {
      mini_z_height = (float)atof(ptr);
      SERIAL_ECHOLNPAIR("mini_level_read_z_height_data==>>z_height: ", mini_z_height);
    }
    else
    {
      SERIAL_ECHOLNPGM("mini_level_read_z_height_data==>>The data is incomplete.");
      return;
    }
  }

  static void mini_level_write_log_to_sd(float *data)
  {
    if (!card.isMounted()) return;

    static SdFile curDir = card.getroot();
    SdBaseFile file;
    char level_log_data[MINI_LEVEL_LOG_BUG_SIZE] = {0};
    (void)snprintf(&level_log_data[0], MINI_LEVEL_LOG_BUG_SIZE, \
                   "SET:0\r\nS00:%f\r\nS01:%f\r\nS02:%f\r\nS10:%f\r\nS11:%f\r\nS12:%f\r\nS20:%f\r\nS21:%f\r\nS22:%f\r\nZ_Height:%f\r\n", \
                   data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], mini_z_height);

    if (file.open(&curDir, MINI_LEVEL_LOG_FILE, O_WRITE))
    {
      file.write(&level_log_data[0], MINI_LEVEL_LOG_BUG_SIZE);
      file.close();
      SERIAL_ECHOLNPGM("mini_level_write_log_to_sd==>>write leveling data successfully");
    }
    else
    {
      SERIAL_ECHOLNPAIR("mini_level_write_log_to_sd==>>open failed, File: ", MINI_LEVEL_LOG_FILE);
    }
  }

  static void mini_level_create_log_file(const char *const fname)
  {
    if (!card.isMounted()) return;

    static SdFile curDir = card.getroot();
    SdBaseFile file;

    if (file.open(&curDir, fname, O_CREAT | O_EXCL | O_RDWR))
    {
      file.close();
    }
  }

  void mini_level_read_log_from_sd()
  {
    if (!card.isMounted())
    {
      return;
    }

    char level_log_data[MINI_LEVEL_LOG_BUG_SIZE] = {0};
    static SdFile curDir = card.getroot();
    SdBaseFile file;

    if (file.open(&curDir, MINI_LEVEL_LOG_FILE, O_READ))
    {
      file.read(&level_log_data[0], MINI_LEVEL_LOG_BUG_SIZE);
      file.close();
      mini_level_read_set_data(level_log_data);
      mini_level_read_level_data(level_log_data, &z_values[0][0]);
      mini_level_read_z_height_data(level_log_data);

      if (mini_level_set_mark || mini_is_eeprom_error)
      {
        mini_level_write_log_to_sd(&z_values[0][0]);
        (void)settings.save();
        SERIAL_ECHOLNPGM("mini_level_read_log_from_sd==>>settings save successfully");
        mini_is_eeprom_error = false;
      }

      SERIAL_ECHOLNPGM("mini_level_read_log_from_sd==>>Set the leveling data successfully");
    }
    else
    {
      SERIAL_ECHOLNPAIR("mini_level_read_log_from_sd==>>open failed, File: ", MINI_LEVEL_LOG_FILE);

      if (mini_level_is_idle())
      {
        mini_level_create_log_file(MINI_LEVEL_LOG_FILE);
        (void)settings.load();
        mini_level_write_log_to_sd(&z_values[0][0]);
      }
    }
  }
#endif

  void mini_level_process(void)
  {
    static bool level_read_once = false;

    if (mini_level_status == MINI_LEVEL_START)
    {
#if ENABLED(SDSUPPORT)

      if (card.isMounted())
      {
        card.removeFile(MINI_LEVEL_LOG_FILE);
      }

#endif
      queue.enqueue_now_P(PSTR("G28"));
      queue.enqueue_now_P(PSTR("G29"));
      queue.enqueue_now_P(PSTR("M500"));
      mini_level_status = MINI_LEVEL_PROCESS;
    }
    else if (mini_level_status == MINI_LEVEL_IDLE)
    {
      if (!level_read_once)
      {
        mini_level_read_log_from_sd();
        level_read_once = true;
        SERIAL_ECHOLNPAIR("mini_level_process==>>get z_height: ", mini_z_height);
      }
    }
  }

  bool mini_level_is_idle(void)
  {
    return mini_level_status == MINI_LEVEL_IDLE;
  }

  bool mini_level_is_check(void)
  {
    return mini_level_status == MINI_LEVEL_CHECK;
  }

  void mini_level_finish(void)
  {
    queue.enqueue_now_P(PSTR("G91"));
    queue.enqueue_now_P(PSTR("G0 F300 Z+20"));
    queue.enqueue_now_P(PSTR("G90"));
    queue.enqueue_now_P(PSTR("G0 F1800 X60"));
    mini_level_status = MINI_LEVEL_IDLE;
    mini_is_level = false;
  }

  void mini_level_set_z_height(void)
  {
    if (mini_level_status == MINI_LEVEL_PROCESS && digitalRead(LEVEL_CHECK) == false)
    {
      mini_level_status = MINI_LEVEL_GET_Z_HEIGHT;
    }

    if (mini_level_status == MINI_LEVEL_GET_Z_HEIGHT && digitalRead(LEVEL_CHECK) == true)
    {
      mini_level_status = MINI_LEVEL_CHECK;
      mini_z_height = stepper.get_position_length(Z_AXIS) + MINI_LEVEL_Z_HEIGHT_OFFSET;
      SERIAL_ECHOLNPAIR("mini_level_set_z_height==>> ", mini_z_height);
    }
  }

  static void mini_level_set_z_values(void)
  {
    uint16_t set_data = parser.value_byte();

    switch (set_data)
    {
    case 00:
      z_values[0][0] = parser.floatval('V');
      break;

    case 01:
      z_values[0][1] = parser.floatval('V');
      break;

    case 02:
      z_values[0][2] = parser.floatval('V');
      break;

    case 10:
      z_values[1][0] = parser.floatval('V');
      break;

    case 11:
      z_values[1][1] = parser.floatval('V');
      break;

    case 12:
      z_values[1][2] = parser.floatval('V');
      break;

    case 20:
      z_values[2][0] = parser.floatval('V');
      break;

    case 21:
      z_values[2][1] = parser.floatval('V');
      break;

    case 22:
      z_values[2][2] = parser.floatval('V');
      break;
    }

    DEBUG_ECHO_START();
    DEBUG_ECHOLNPAIR("mini_level_set_z_values==>>S", set_data, "Settings saved successfully");
  }

  static bool mini_level_check_z_values(float *data)
  {
    for (int i = 0; i < 9; i++)
    {
      if (isnan(data[i]))
      {
        SERIAL_ECHOLNPGM("mini_level_check_z_values==>>Set the leveling data unsuccessful");
        return true;
      }
    }

    return false;
  }

  bool mini_level_check_data(void)
  {
    if (parser.seen('S'))
    {
      mini_level_set_z_values();
      return false;
    }

    if (mini_level_check_z_values(&z_values[0][0]))
    {
      return false;
    }

    return true;
  }
}

#endif


