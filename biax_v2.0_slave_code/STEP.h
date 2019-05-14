#ifndef __STEP__H__
#define __STEP__H__

#include <Arduino.h>
extern void init_motor_io();
extern int motor_start(double Speed);
extern void motor_stop();
extern void reset_zero();
extern void motor_disable();
extern void motor_enable();
extern void reverse_dir();
#endif
