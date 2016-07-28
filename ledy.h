#include <avr/io.h>
#include "integer.h"
#include <util/delay.h>  
#define RED_LED_ON		PORTA |= 1<<PA0
#define RED_LED_OFF		PORTA &= ~(1<<PA0)
#define WHITE_LED_ON		PORTA |= 1<<PA1
#define WHITE_LED_OFF		PORTA &= ~(1<<PA1)

void blinkRedLEDPaliKrotko(void);
void blinkRedLEDPaliDlugo(void);
void blinkWhiteLED(void);
void blinkRedWhiteLED(void);
void blinkWhiteLEDnrazy(BYTE n);
void blinkRedLEDnrazy(BYTE n);

