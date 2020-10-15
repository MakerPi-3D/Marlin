
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

#include "../module/temperature.h"

namespace sg_mini
{

  void mini_fan_process()
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

}

#endif


