uint8_t x_data[4];
uint8_t y_data[4];

void RecieveXdata();
void RecieveYdata();
void OverRange();
void x_start(uint8_t mode);
void y_start(uint8_t mode);
void print_data();

char comchar;
bool if_start_run=0,y_enable=1;

void setup() {
  DDRF=0x00;             //接收x轴数据
  DDRK=0x00;             //接收y轴数据
  bitSet(DDRJ,0);        //x轴数据传输控制
  bitSet(DDRB,0);        //x轴启停控制
  bitSet(DDRA,0);        //y轴数据传输控制
  bitSet(DDRC,0);        //y轴启停控制
  DDRD=0x00;
  Serial.begin(115200);
  Serial2.begin(115200);
  attachInterrupt(2, OverRange,RISING);
  attachInterrupt(3, RecieveXdata,RISING);
  attachInterrupt(4, RecieveYdata,RISING);
  TCCR1A=0x00;
  TCCR1B=0b00001101;
  TCNT1=0;
  OCR1A=3124;
  Serial.println("format: x-force,y-force, unit:0.1g, f=5hz");
}

void loop() {
  while(Serial.available()>0){
    comchar = Serial.read();    //读串口第一个字节
    switch(comchar){
      case '0':x_start(0);                          //模式0，电机停止
               if(y_enable)
                 y_start(0);
               TIMSK1 &= ~(1 << OCIE1A);break;
      case '1':x_start(1);                          //模式1，恒速度加载
               if(y_enable)
                 y_start(1);
               TIMSK1 |= (1 << OCIE1A);break;
      case '2':x_start(2);                          //模式2，恒力加载
               if(y_enable)
                 y_start(2);
               TIMSK1 |= (1 << OCIE1A);break;
      case '3':x_start(3);                          //模式3，预加载
               if(y_enable)
                 y_start(3);
               TIMSK1 |= (1 << OCIE1A);Serial.println("预加载");break;
      case '4':x_start(4);                         //模式4，输出数据
               if(y_enable)
                 y_start(4);
               TIMSK1 |= (1 << OCIE1A);break; //enable timer1 启用输出数据中断程序
      case '5':x_start(5);                         //模式5，停止输出数据
               if(y_enable)
                 y_start(5);
               TIMSK1 &= ~(1 << OCIE1A);break;//disable timer1
      case '6':x_start(6);                         //模式6，位置归零
               if(y_enable)
                 y_start(6);
               Serial.println("位置归零");break;
      case '7':x_start(7);                         //模式7，校准
               if(y_enable)
                 y_start(7);
               Serial.println("传感器拉力归零");break;
      case '8':x_start(8);                         //模式8，反向
               if(y_enable)
                 y_start(8);
               Serial.println("反向");break;
      
      case 'a':x_start(20);break;  //设置x轴速度为0
      case 'b':x_start(21);break;
      case 'c':x_start(22);break;
      case 'd':x_start(23);break;
      case 'e':x_start(24);break;
      case 'f':x_start(25);break;
      case 'g':x_start(26);break;
      case 'h':x_start(27);break;
      case 'i':x_start(28);break;
      case 'j':x_start(29);break;
      case 'k':x_start(30);break;
      case 'l':x_start(31);break;
      case 'm':x_start(32);break;
      case 'n':x_start(33);break;
      case 'o':x_start(40);break;  //设置x轴力为0
      case 'p':x_start(41);break;
      case 'q':x_start(42);break;
      case 'r':x_start(43);break;
      case 's':x_start(44);break;
      case 't':x_start(45);break;
      case 'u':x_start(46);break;
      case 'v':x_start(47);break;
      case 'w':x_start(48);break;
      case 'A':y_start(20);break;  //设置y轴速度为0
      case 'B':y_start(21);break;
      case 'C':y_start(22);break;
      case 'D':y_start(23);break;
      case 'E':y_start(24);break;
      case 'F':y_start(25);break;
      case 'G':y_start(26);break;
      case 'H':y_start(27);break;
      case 'I':y_start(28);break;
      case 'J':y_start(29);break;
      case 'K':y_start(30);break;
      case 'L':y_start(31);break;
      case 'M':y_start(32);break;
      case 'N':y_start(33);break;
      case 'O':y_start(40);break;   //设置y轴力为0
      case 'P':y_start(41);break;
      case 'Q':y_start(42);break;
      case 'R':y_start(43);break;
      case 'S':y_start(44);break;
      case 'T':y_start(45);break;
      case 'U':y_start(46);break;
      case 'V':y_start(47);break;
      case 'W':y_start(48);break;
      
      case 'X':y_enable=0;break;    //停用y轴（单轴拉伸）
      case 'Y':y_enable=1;break;    //启用y轴（双轴拉伸）
    }
  }
  delay(2); 
  if(if_start_run){
    if_start_run=0;
    print_data();
  }
}

ISR(TIMER1_COMPA_vect){
  if_start_run=1;
}

void RecieveXdata(){
  x_data[PINF&0x0f]=PINF>>4;
}

void RecieveYdata(){
  y_data[PINK&0x0f]=PINK>>4;
}

void OverRange(){
  
}

void y_start(uint8_t mode){
  DDRK=0xff;
  PORTK=mode;
  PORTC=0xFF;
  delayMicroseconds(25);
  PORTC=0x00;
  DDRK=0x00;
}

void x_start(uint8_t mode){
  DDRF=0xff;
  PORTF=mode;
  PORTB=0xFF;
  delayMicroseconds(25);
  PORTB=0x00;
  DDRF=0x00;
}

void print_data(){
  Serial.print("x");
  for(int i=3;i>=0;i--)
    Serial.print(x_data[i]);
  Serial.print("y");
  for(int i=3;i>=0;i--)
    Serial.print(y_data[i]);
  Serial.print('\n');
}
