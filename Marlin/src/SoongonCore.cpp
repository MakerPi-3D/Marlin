/**
 *
 */

#include "SoongonCore.h"
#include "sg/mini.h"

#if ENABLED(SOONGON_I3_SECTION_CODE)

#include "module/temperature.h"

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
}

#endif

void SoongonCore::run()
{
#if ENABLED(SOONGON_MINI_SECTION_CODE)
  sg_mini::mini_run();
#elif ENABLED(SOONGON_I3_SECTION_CODE)
  i3_run();
#endif
}

SoongonCore sg_core;









