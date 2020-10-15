
#include "mini.h"

#if ENABLED(SOONGON_MINI_SECTION_CODE)

namespace sg_mini
{
  void mini_key_check(MINI_KEY_T &key)
  {
    if (digitalRead(key.KeyPin) == 0)
    {
      key.KeyAge++;

      if (key.KeyAge > 1)
      {
        key.KeyPress = 1;
      }

      if (key.KeyAge > 9)
      {
        key.KeyHold = 1;
      }

      if ((key.KeyHold) && (key.KeyProcessed == 0))
      {
        key.KeyProcessed = 1;
        key.fun(KEY_LONG_PRESS);
      }
    }
    else
    {
      if ((key.KeyPress) && (key.KeyProcessed == 0))
      {
        key.KeyProcessed = 1;
        key.fun(KEY_SHORT_PRESS);
      }

      key.KeyAge = 0;
      key.KeyPress = 0;
      key.KeyHold = 0;
      key.KeyProcessed = 0;
    }
  }

  void mini_key_init()
  {
    mini_key_print_init();
    //filament startup not run
    mini_key_load_init();
    mini_key_unload_init();
  }

  void mini_key_process()
  {
    if (mini_is_level) return;

    if (!mini_is_load && !mini_is_load)
    {
      mini_key_print_check();
    }

    if (!mini_is_print && !mini_is_load)
    {
      mini_key_load_check();
    }

    if (!mini_is_print && !mini_is_load)
    {
      mini_key_unload_check();
    }
  }

}

#endif


