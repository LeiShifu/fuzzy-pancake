#include "STEP.h"
//steeper_STEP1 PB1 接16位定时器 直接接M0
//steeper_STEP2 PB0 归零用
//steeper_DIR   PB3
//steeper_EN    PB2
//steeper_Source PB5

int motor_start(double Speed){
  if(Speed>0&&Speed<0.5){
    int gap;
    gap=11.71875/Speed-1;
    cli();
    TCCR1A = 0x00 ;         //
    TCCR1B = 0x00 ;         //
    TCCR1B = _BV(WGM12)|_BV(CS10)|_BV(CS11);
    TCNT1 = 0;
    OCR1A = gap;
    TCCR1A = _BV(COM1A0);     
    sei();
    return 1;
  }
  else if(Speed<=0){
    TCCR1A =0X00; 
    return 0;
  }
  else 
    return 0;
}

void motor_stop(){
  TCCR1A =0X00; 
}

void init_motor_io(){
  bitSet(DDRB,0);           //PB0,PB1输出脉冲
  bitSet(DDRB,1);
  bitSet(DDRB,2);           //PB2 EN电机使能，低有效
  bitSet(DDRB,3);           //PB3 DIR电机方向选择，1为正向(拉伸)
  bitSet(DDRB,5);           //PB5 step源选择，PB5=0时M0M1源为PB1，PB5=1时M0源为PB1，M1源为PB0
  PORTB=0b001000;
}

void reset_zero(){
  bitClear(PORTB,2);
  bitClear(DDRD,0);        //M0归零电平指示，高为到位
  bitClear(DDRD,1);        //M1归零电平指示，高为到位
  bitClear(PORTB,3);       //电机反转
  bitSet(PORTB,5);         //PB5=1时M0源为PB1，M1源为PB0
  bool M0=1,M1=1;
  while(M1||M0){
    M0=!bitRead(PIND,0);
    M1=!bitRead(PIND,1);
    if(M0){
      bitSet(PORTB,1);
      bitClear(PORTB,1);
    }    
    if(M1){
      bitSet(PORTB,0);
      bitClear(PORTB,0);
    }
    delayMicroseconds(60);
  }
  bitSet(PORTB,3);           //电机正转
  bitClear(PORTB,5);         //PB5=0时M0,M1源为PB1
  for(int i=1066;i>0;i--){
    bitSet(PORTB,1);
    bitClear(PORTB,1);
    delayMicroseconds(200);
  }
  bitSet(DDRD,1);
  TCCR1A =0X00; 
}

void reverse_dir(){
  bool dir=bitRead(PORTB,3);
  if(dir)
    bitClear(PORTB,3);
  else
    bitSet(PORTB,3);
}

void motor_disable(){
  bitSet(PORTB,2);
}

void motor_enable(){
  bitClear(PORTB,2);
}
