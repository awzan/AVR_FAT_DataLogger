#include <util/delay.h>
#include "przyciski.h"

uint8_t czy_klawisz_wcisniety1(void)
{
  if(KEY_DOWN1)
  {
    _delay_ms(79);
    if (KEY_DOWN1)
      return 1;
  }
  return 0;
}
uint8_t czy_klawisz_wcisniety2(void)
{
  if(KEY_DOWN2)
  {
    _delay_ms(79);
    if (KEY_DOWN2)
      return 1;
  }
  return 0;
}
