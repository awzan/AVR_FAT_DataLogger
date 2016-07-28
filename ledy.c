#include "ledy.h"

void blinkRedLEDPaliKrotko(void)
{
  while(1)  //blink red LED continuously, if error
  {
    RED_LED_ON;
    _delay_ms(100);
    RED_LED_OFF;
    _delay_ms(3000);
  }

}
void blinkRedLEDPaliDlugo(void)
{
  while(1)  //blink red LED continuously, if error
  {
    RED_LED_ON;
    _delay_ms(3000);
    RED_LED_OFF;
    _delay_ms(100);
  }

}
void blinkWhiteLED(void)
{
  while(1)  //blink red LED continuously, if error
  {
    WHITE_LED_ON;
    _delay_ms(1000);
    WHITE_LED_OFF;
    _delay_ms(1000);
  }

}

void blinkRedWhiteLED(void)
{
  while(1)  //blink red LED continuously, if error
  {
    WHITE_LED_ON;
    _delay_ms(1000);
    WHITE_LED_OFF;
    _delay_ms(1000);
    RED_LED_ON;
    _delay_ms(1000);
    RED_LED_OFF;
    _delay_ms(1000);
  }

}

void blinkWhiteLEDnrazy(BYTE n)
{
  n=n+1;
  while(n>0)  //blink red LED continuously, if error
  {
    WHITE_LED_ON;
    _delay_ms(1000);
    WHITE_LED_OFF;
    _delay_ms(1000);
    n--;
  }
}
void blinkRedLEDnrazy(BYTE n)
{
  n=n+1;
  while(n>0)  //blink red LED continuously, if error
  {
    RED_LED_ON;
    _delay_ms(300);
    RED_LED_OFF;
    _delay_ms(300);
    n--;
  }
}
