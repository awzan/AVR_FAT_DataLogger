
#include <avr/io.h>
#define KEY_PIN1 (1<<PA5)
#define KEY_DOWN1 !(PINA & KEY_PIN1)
#define KEY_PIN2 (1<<PA6)
#define KEY_DOWN2 !(PINA & KEY_PIN2)

uint8_t czy_klawisz_wcisniety1(void);

uint8_t czy_klawisz_wcisniety2(void);

