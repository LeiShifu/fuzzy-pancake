#ifndef __HX711__H__
#define __HX711__H__

#include <Arduino.h>

extern void Init_Hx711();
extern void calibrate();

extern unsigned long Hx711_Read(void);
extern long Get_Force(void);

#endif
