#include "HX711.h"
#include "STEP.h"

void runcountrol();
void SendForce();
void init_conmunication_io();
void start_run_repetition();
void start_run_force(double force_speed);
long get_data();

uint8_t recieved_data[8];
uint8_t force_data[4];
uint8_t Bit;
double SetSpeed=0,SetForce=0,SetPreForce=100;

bool motor_start_flag=0;
bool start_run_flag=0;
bool start_run_repetition_flag=0;//预加载
bool start_run_force_flag=0;     //恒力加载
bool reset_zero_flag=0;          //位置归零
bool calibrate_flag=0;           //校准
bool reverse_dir_flag=0;         //反向

void setup(){
  init_motor_io();
  Init_Hx711();
  init_conmunication_io();
  attachInterrupt(0, runcountrol,RISING);
  calibrate();
  reset_zero();
}

void loop(){
  if(motor_start_flag){
    long Force=Get_Force();
    if(Force>0){
      force_data[0]=Force%10;
      Force/=10;
      force_data[1]=Force%10;
      Force/=10;
      force_data[2]=Force%10;
      Force/=10;
      force_data[3]=Force%10;
    }
    else
      force_data[0]=force_data[1]=force_data[2]=force_data[3]=0; 
    SendForce();
    delay(110);
  }
  if(start_run_repetition_flag){  //预加载
    start_run_repetition_flag=0;
    start_run_repetition();
  }
  if(start_run_force_flag){
    start_run_force_flag=0;     //恒力加载
    start_run_force(SetSpeed);
  }
  if(reset_zero_flag){
   reset_zero_flag=0;          //位置归零
   reset_zero();
  }
  if(calibrate_flag){
    calibrate_flag=0;           //校准
    calibrate();
  }
  if(reverse_dir_flag){
    reverse_dir_flag=0;         //反向
    reverse_dir();
  }
}
void runcountrol(){
  DDRD=0x00;
  DDRC=0xf0&DDRC;
  delayMicroseconds(1);
  DDRD=0x00;
  DDRC=0xf0&DDRC;
  delayMicroseconds(1);
  switch((PIND>>4&0x0f)|(PINC<<4&0xf0)){
    case 0:motor_stop();motor_start_flag=0;break;          //模式0，电机停止
    case 1:motor_start_flag=motor_start(SetSpeed);break;    //模式1，恒速度加载
    case 2:start_run_force_flag=1;break;     //模式2，恒力加载
    case 3:start_run_repetition_flag=1;break;//模式3，预加载
    case 4:motor_start_flag=1;break;        //模式4，输出数据
    case 5:motor_start_flag=0;break;        //模式5，停止输出数据
    case 6:reset_zero_flag=1;break;          //模式6，位置归零
    case 7:calibrate_flag=1;break;           //模式7，校准
    case 8:reverse_dir_flag=1;break;         //模式8，反向
    case 20:SetSpeed=0;break;         //设置速度为0
    case 21:SetSpeed+=1;break;        //速度加1mm/s
    case 22:SetSpeed+=0.5;break;      //速度加0.5mm/s
    case 23:SetSpeed+=0.2;break;      //速度加0.2mm/s
    case 24:SetSpeed+=0.1;break;      //速度加0.1mm/s
    case 25:SetSpeed+=0.05;break;
    case 26:SetSpeed+=0.02;break;
    case 27:SetSpeed+=0.01;break; 
    case 28:SetSpeed+=0.005;break;
    case 29:SetSpeed+=0.002;break;
    case 30:SetSpeed+=0.001;break;
    case 31:SetSpeed+=0.0005;break;
    case 32:SetSpeed+=0.0002;break;
    case 33:SetSpeed+=0.0001;break; 
    case 40:SetForce=0;break;          //设置应力松弛时最大拉力为0
    case 41:SetForce+=500;break;       //最大拉力加50g
    case 42:SetForce+=200;break;       //模式1，恒速度加载
    case 43:SetForce+=100;break;       //模式1，恒速度加载
    case 44:SetForce+=50;break;
    case 45:SetForce+=20;break; 
    case 46:SetForce+=10;break;
    case 47:SetPreForce=0;break;       //设置预拉伸拉力为0
    case 48:SetPreForce+=10;break;     //预拉伸拉力加1g
  }
 DDRD=0b11110000|DDRD;
 DDRC=0b00001111|DDRC;
}

void start_run_force(double force_speed){
  int i=12000;
  long ooo=0;
  motor_start(force_speed);
  while(get_data()<SetForce){
    SendForce();
    delay(100);
  }
  force_speed=force_speed/2;
  while(i>0){
    ooo=get_data();
    if(ooo>SetForce*1.01){
      force_speed=force_speed/2;
      motor_start(force_speed);
    }
    else if(ooo<SetForce*0.99){
      force_speed=force_speed*1.5;
      motor_start(force_speed);
    }
    SendForce();
    delay(100);
    i--;
  }
  motor_stop();
}

long get_data(){
    long nega=Get_Force();
    if(nega>0){
      force_data[0]=nega%10;
      nega/=10;
      force_data[1]=nega%10;
      nega/=10;
      force_data[2]=nega%10;
      nega/=10;
      force_data[3]=nega%10;
    }
    else
      force_data[0]=force_data[1]=force_data[2]=force_data[3]=0; 
    return nega;
}

void start_run_repetition(){
  motor_start(0.05);
  for(int i=0;i<3;i++){
  while(Get_Force()<SetPreForce){
    delay(100);
  }
  reverse_dir();
  while(Get_Force()>0){
    delay(100);
  }
  reverse_dir();
  }
  motor_stop();
}

void SendForce(){
  PORTD=(0x01&PORTD)|0x00;
  PORTC=(0x10&PORTC)|force_data[0];
  bitSet(PORTB,4);
  delayMicroseconds(10);
  bitClear(PORTB,4);
  PORTD=(0x01&PORTD)|0x10;
  PORTC=(0x10&PORTC)|force_data[1];
  bitSet(PORTB,4);
  delayMicroseconds(10);
  bitClear(PORTB,4);
  PORTD=(0x01&PORTD)|0x20;
  PORTC=(0x10&PORTC)|force_data[2];
  bitSet(PORTB,4);
  delayMicroseconds(10);
  bitClear(PORTB,4);
  PORTD=(0x01&PORTD)|0x30;
  PORTC=(0x10&PORTC)|force_data[3];
  bitSet(PORTB,4);
  delayMicroseconds(10);
  bitClear(PORTB,4);
}

void init_conmunication_io(){
  bitSet(DDRC,0);
  bitSet(DDRC,1);
  bitSet(DDRC,2);
  bitSet(DDRC,3);
  bitSet(DDRD,4);
  bitSet(DDRD,5);
  bitSet(DDRD,6);
  bitSet(DDRD,7);
  bitSet(DDRB,4);           //从机发送数据指示位
  bitClear(DDRD,2);         //从机启停控制中断，int0
  bitClear(DDRD,3);         //从机接收数据控制中断，int1
}

