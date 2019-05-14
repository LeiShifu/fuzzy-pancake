//HX711_SCK_X PC5 OUTPUT
//HX711_DT_X  PC4 INPUT
//***************************************************
#include "HX711.h"

#define GapValue 158.2
long Hx711_cali=0;

void Init_Hx711(){
	DDRC = _BV(5);
  PORTC = B00000000;
}

void calibrate(){ 
  Hx711_cali=0;
  for(int i=0;i<10;i++){
    Hx711_cali+=Hx711_Read();
    delay(250);
  }
  Hx711_cali=Hx711_cali*0.1;
}

long Get_Force(){
  long Force=Hx711_Read()-Hx711_cali;
  Force=Force/GapValue;
  return Force;
}

unsigned long Hx711_Read(void)  //增益128
{
  unsigned long count; 
  unsigned char i;
  PORTC = ( 1<<4 ) | PORTC;
  PORTC = ~( 1<<5 ) & PORTC;
  count=0; 
  while( (PINC & (1<<4) )>>4 );
    for(i=0;i<24;i++)
  { 
      PORTC = ( 1<<5 ) | PORTC;
      count=count<<1; 
      PORTC= ~( 1<<5 ) & PORTC;
      if( (PINC & (1<<4) )>>4 )
         count++; 
  } 
  PORTC = ( 1<<5 ) | PORTC;
  count ^= 0x800000;
  PORTC= ~( 1<<5 ) & PORTC;
  return(count);
}

