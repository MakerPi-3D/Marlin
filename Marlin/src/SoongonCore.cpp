/**
 *
 */

#include "SoongonCore.h"
#include "sg/mini.h"

#if ENABLED(SDSUPPORT)
  #include "sd/cardreader.h"
#endif

#if ENABLED(SOONGON_I3_SECTION_CODE)

#include "module/temperature.h"
#include "lcd/ultralcd.h"

static volatile millis_t lcd_init_timeout = 0;

void SoongonCore::i3_run(void)
{
  // Turn on the fan when the nozzle temperature of the machine is greater than 50 ?
  if (thermalManager.fan_speed[0] == 0 && thermalManager.degHotend(0) > 50)
  {
    thermalManager.fan_speed[0] = 255;
    SERIAL_ECHOLNPGM("Fan on, speed s255");
  }
  else if (thermalManager.fan_speed[0] > 0 && thermalManager.degHotend(0) <= 50)
  {
    thermalManager.fan_speed[0] = 0;
    SERIAL_ECHOLNPGM("Fan off, speed s0");
  }

  if (ELAPSED(millis(), lcd_init_timeout))
  {
    lcd_init_timeout = millis() + 60*1000;
    ui.init_lcd();
    ui.update();
  }
}

#endif

void SoongonCore::run()
{
#if ENABLED(SOONGON_MINI_SECTION_CODE)
  sg_mini::mini_run();
#elif ENABLED(SOONGON_I3_SECTION_CODE)
  i3_run();
#endif

#if ENABLED(SDSUPPORT)
  static bool isRunOnce = true;
  if(isRunOnce && card.isMounted())
  {
    static SdFile curDir = card.getroot();
    SdBaseFile file;
    if (file.open(&curDir, "RECYCLER", O_READ))
    {
      bool result = file.rmRfStar();
      file.close();
      SERIAL_ECHOLNPAIR("Remove dir RECYCLER:", result?"Done":"Failed");
    }
    isRunOnce = false;
  }
#endif
}

SoongonCore sg_core;









