/*----------------------------------------------------------------------*/
/* Temperature Data Logger on FATfs formatted SD cards */
/*----------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include "rtc.h"
#include "ds18b20.h"
#include "ledy.h"
#include "przyciski.h"


#define SCK PB7
#define MOSI PB5
#define CS PB4
#define MISO PB6
static volatile int Stan = 0;

static volatile uint16_t hs = 0; //number of half seconds since the power up

void configure_timers()
{
  // 8 bit  TIMER0 configuration, normal mode

  TCCR0 = 1 << CS02 | 1<<CS00;// F_CPU / 1024 = 7812.5 Hz
  TIMSK |= 1 << TOIE0;// TIMER0_OVF Enable

  // 16 bit  TIMER1 configuration, CTC mode 

  TCCR1B |= (1 << CS12); // F_CPU / 256, assuming F_CPU = 8 000 000 hz, ~31 250 Hz
  TCCR1B |= (1 << WGM12); // CTC mode  
  OCR1A = 15624; // assuming F_CPU = 8 000 000 hz, interrupt every ~1/2sec
  TIMSK |= 1 << OCIE1A; // Timer/Counter1 Output Compare A Match Interrupt Enable.

  // 8 bit  TIMER2 configuration, CTC mode 

  TCCR2 |= WGM21; // CTC mode enable, compare to value set in OCR2
  TCCR2 = 1 << CS02 | 1<< CS01 | 1<<CS00;// F_CPU / 1024 = 7812.5 Hz
  OCR2 = 158-1;	// Timer2: 50Hz interval (OC2)
  // TCCR2 = 0b00001101;
  TIMSK |= 1<<OCIE2;	// Timer/Counter2 Output Compare Match Interrupt Enable

}

ISR( TIMER0_OVF_vect )
{ 

  if( czy_klawisz_wcisniety1() )
    Stan = 1;
  if( czy_klawisz_wcisniety2() && Stan == 1)
    Stan = 2;
}
ISR( TIMER1_COMPA_vect )
{
  hs++;
}

//DWORD acc_size;				/* Work register for fs command */
//WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[_MAX_LFN+1];
#endif


char Line[128];				/* Console input buffer */

FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
BYTE Buff[256];			/* Working buffer */

volatile WORD Timer;		/* 100Hz increment timer */



#if _MULTI_PARTITION != 0
const PARTITION Drives[] = { {0,0}, {0,1} };
#endif



ISR(TIMER2_COMP_vect, ISR_NOBLOCK)
{
  Timer++;			/* Performance counter for this module */
  disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}



/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */


DWORD get_fattime ()
{
  RTC rtc;


  // Get local time 
  //rtc_gettime(&rtc);
  rtc.year = 2016;
  rtc.month = 7;
  rtc.mday = 10;
  rtc.hour = 16;
  rtc.min = 1;
  rtc.sec = 0;

  // Pack date and time into a DWORD variable 
  return	  ((DWORD)(rtc.year - 1980) << 25)
    | ((DWORD)rtc.month << 21)
    | ((DWORD)rtc.mday << 16)
    | ((DWORD)rtc.hour << 11)
    | ((DWORD)rtc.min << 5)
    | ((DWORD)rtc.sec >> 1);
}


void bufferFill(BYTE* buffer, char* napis, int dlugosc)
{
  int i;
  for(i = 0; i < 5-1; i++)
    buffer[i] = (BYTE)napis[i];
  buffer[5] = (BYTE)'\r';
  buffer[6] = (BYTE)'\n';
  //buffer[7] = (BYTE)'\0';
}

static void IoInit ()
{
  //PORTA = 0b11111111;	// Port A
  PORTB = 0b10110000; // Port B
  DDRB  = 0b11000000;
  // Configure pins for SPI 
  DDRB |= ((1<<CS) | (1<<MOSI) | (1<<SCK));
  PORTB |= (1<<CS); 
  PORTB |= (1<<MISO);
  SPCR |= ((1<<SPE) | (1<<MSTR)); // initialize hardware SPI support
  
  // Configure pins for LEDs

  DDRA |= ((1<<PA0) | (1<<PA1));
  //PORTC = 0b11111111;	// Port C

  // Configure pins for switches SW1 and SW2 <cto: 0 and 1>
  // Why is it pulled up to Vcc however?
  PORTD |= 1<<PD0 | 1 << PD1;
  //PORTD = 0b11111111; // Port D

  rtc_init();			// Initialize RTC

  sei();
}



/*-----------------------------------------------------------------------*/
/* Main                                                                  */


int main (void)
{
  IoInit();
  configure_timers();
  RED_LED_OFF;
  WHITE_LED_OFF;
  //char *ptr, *ptr2;
  long p1;
  //BYTE res, tryb, b1, *bp;
  BYTE tryb;
  //WORD w1;
  //UINT s1, s2;
  //DWORD ofs, sect = 0;
  //const BYTE ft[] = {0,12,16,32};
  //RTC rtc;
  //FATFS *fs;
  FIL file1;		// File object 
  //char* napis = "aaaaa";
  char  temp_str[10]; // String to put temperature in
  int dlugosc = 5; // Limit for buffer, probably unneeded
  int w;
  //double d;


#if _USE_LFN
  Finfo.lfname = Lfname;
  Finfo.lfsize = sizeof(Lfname);
#endif
  //blinkWhiteLEDnrazy(FR_OK);
  //d = ds18b20_gettemp()*100.0;
  //blinkWhiteLEDnrazy(FR_OK);
  w = (int) ds18b20_gettemp()*100.0;

  //blinkWhiteLEDnrazy(FR_OK);
  itoa(w,temp_str ,10);

  //blinkWhiteLEDnrazy(FR_OK);
  p1 = 0;
  //blinkWhiteLEDnrazy(FR_OK);
  tryb = 0b00001011;
  //blinkWhiteLEDnrazy(FR_OK);
  bufferFill(Buff, temp_str, dlugosc);
  //blinkWhiteLEDnrazy(FR_OK);
  /*DDRC &= ~(1<<PC2);
  //PORTC |= (1<<PC2);

  if(!(PINC & (1<<PC2)))
  blinkRedWhiteLED();

  blinkWhiteLED();*/
  blinkWhiteLEDnrazy(0);
  DSTATUS StatusKarty = disk_initialize((BYTE)p1) ;

  if( (StatusKarty & STA_NOINIT) == STA_NOINIT )
  {
    blinkRedLEDPaliKrotko();		
  }

  if( (StatusKarty & STA_PROTECT) == STA_PROTECT) // impossible to be true, sta_protect forced to low
  {
    blinkRedWhiteLED();		
  }
  if(f_mount((BYTE)p1, &Fatfs[p1]) != FR_OK)
    blinkRedLEDPaliDlugo();	


  if( (f_open(&file1, "T.txt", tryb) ) == FR_OK)
  {
    blinkRedLEDnrazy(0);
    while(1)
    {
      if(Stan == 1)
      {
        blinkRedLEDnrazy(0);

        //d = ds18b20_gettemp()*100.0;
        w = (int) ds18b20_gettemp()*100.0; // Fatfs library isn't able to deal with floats. Conv to int
        itoa(w,temp_str ,10);
        //itoa(ds/10,ds_str ,10);

        //if((f_write(&file1, Buff, dlugosc, &s2) & FR_OK) == FR_OK);
        //		blinkRedLEDnrazy(FR_OK);
        if((f_printf(&file1, "%4d",hs) != EOF ))
        {
          f_printf(&file1, " - ");
          //blinkWhiteLEDnrazy(FR_OK);
        }

        if((f_printf(&file1, temp_str) != EOF ))
        {
          f_printf(&file1, "\n");
          //blinkRedLEDnrazy(FR_OK);
        }

      }
      if(Stan == 2)
      {
        //blinkWhiteLEDnrazy(FR_OK);
        if((f_close(&file1) & FR_OK ) == FR_OK) 
          f_mount(0,0);
        blinkRedLEDPaliDlugo();
      }

    }
  }
  //if((f_mount(NULL, &Fatfs[p1]) & FR_OK == FR_OK))
  //blinkRedWhiteLED();

  return 0;
}

