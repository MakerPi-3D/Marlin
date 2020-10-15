/**
 *
 */

#include "SoongonCore.h"
#include "sg/mini.h"

#if ENABLED(SOONGON_I3_SECTION_CODE)

void SoongonCore::i3(void)
{
}

#endif

void SoongonCore::run()
{
#if ENABLED(SOONGON_MINI_SECTION_CODE)
  sg_mini::mini_run();
#endif
}

SoongonCore sg_core;









