/*
APteck
------
Written By:        Anthony Pirotta.
Date:              20/05/2023.
Revised:           20/05/2023.
Name Of Code:      OV7670_FIFO_Camera.
Written For:       RP2040 based boards or PI Picos using the Raspberry Pi Pico tool in the Arduino IDE.
Purpose Of Code:   Writing and reading camera data from a FIFO using a PI Pico or an RP2040 based board.
*/


#include <Wire.h>
#include <SPI.h>
#include <SD.h>


#define CAMERA (0x42 >> 1)


#define REG_GAIN  0x00
#define REG_BLUE  0x01
#define REG_RED  0x02
#define REG_COM1  0x04
#define REG_VREF  0x03
#define REG_COM4  0x0d
#define REG_COM5  0x0e
#define REG_COM6  0x0f
#define REG_AECH  0x10
#define REG_CLKRC  0x11
#define REG_COM7  0x12
#define COM7_RGB  0x04
#define REG_COM8  0x13
  #define COM8_FASTAEC  0x80    // Enable fast AGC/AEC
  #define COM8_AECSTEP  0x40    // Unlimited AEC step size
  #define COM8_BFILT  0x20    // Band filter enable
  #define COM8_AGC  0x04    // Auto gain enable
  #define COM8_AWB  0x02    // White balance enable
#define COM8_AEC  0x0
#define REG_COM9  0x14
#define REG_COM10  0x15
#define REG_COM14  0x3E
#define REG_SCALING_DCWCTR  0x72
#define REG_SCALING_PCLK_DIV  0x73
#define REG_COM11  0x3B
#define COM11_NIGHT  0x80
#define COM11_NMFR  0x60
#define COM11_HZAUTO  0x10
#define COM11_50HZ  0x08
#define COM11_EXP  0x0
#define REG_TSLB  0x3A
#define REG_RGB444  0x8C
#define REG_COM15  0x40
#define COM15_RGB565  0x10
#define COM15_R00FF  0xc0
#define REG_HSTART  0x17
#define REG_HSTOP  0x18
#define REG_HREF  0x32
#define REG_VSTART  0x19
#define REG_VSTOP  0x1A
#define REG_COM3  0x0C
#define REG_MVFP  0x1E
#define REG_COM13  0x3d
#define COM13_UVSAT  0x40
#define SCALING_DCWCTR  0x72
#define SCALING_PCLK_DIV  0x73
#define REG_BD50MAX  0xa5
#define REG_BD60MAX  0xab
#define REG_AEW  0x24
#define REG_AEB  0x25
#define REG_VPT  0x26
#define REG_HAECC1  0x9f
#define REG_HAECC2  0xa0
#define REG_HAECC3  0xa6
#define REG_HAECC4  0xa7
#define REG_HAECC5  0xa8
#define REG_HAECC6  0xa9
#define REG_HAECC7  0xaa
#define REG_COM12  0x3c
#define REG_GFIX  0x69
#define REG_COM16  0x41
#define COM16_AWBGAIN  0x08
#define REG_EDGE  0x3f
#define REG_REG76  0x76
#define ADCCTR0  0x20
#define REG_SCALING_YSC 0x71
#define REG_SCALING_XSC 0x70




uint8_t myImage[80][60];

const int VSYNC = 10;   //vertical sync

const int RRST = 11;    //read reset
const int WRST = 12;    //write reset
const int RCK = 13;     //read clock
const int WR = 14;      //write flag
const int LED = 25;     //LED Pin



unsigned long timeBegin = 0;
unsigned long duration = 0;




 void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data){
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();

 }



void frameControl(int hStart, int hStop, int vStart, int vStop)
  {
    I2CwriteByte(CAMERA, REG_HSTART, hStart >> 3);
    I2CwriteByte(CAMERA, REG_HSTOP,  hStop >> 3);
    I2CwriteByte(CAMERA, REG_HREF, ((hStop & 0b111) << 3) | (hStart & 0b111));

    I2CwriteByte(CAMERA, REG_VSTART, vStart >> 2);
    I2CwriteByte(CAMERA, REG_VSTOP, vStop >> 2);
    I2CwriteByte(CAMERA, REG_VREF, ((vStop & 0b11) << 2) | (vStart & 0b11));
  }



  

void saturation(int s)  //-2 to 2
  {
    //color matrix values
    I2CwriteByte(CAMERA, 0x4f, 0x80 + 0x20 * s);
    I2CwriteByte(CAMERA, 0x50, 0x80 + 0x20 * s);
    I2CwriteByte(CAMERA, 0x51, 0x00);
    I2CwriteByte(CAMERA, 0x52, 0x22 + (0x11 * s) / 2);
    I2CwriteByte(CAMERA, 0x53, 0x5e + (0x2f * s) / 2);
    I2CwriteByte(CAMERA, 0x54, 0x80 + 0x20 * s);
    I2CwriteByte(CAMERA, 0x58, 0x9e);  //matrix signs
  }



void setup() {

inline void readReset() __attribute__((always_inline));
inline void writeEnable() __attribute__((always_inline));
inline void writeDisable() __attribute__((always_inline));
inline void writeReset() __attribute__((always_inline));


  Serial.begin(460800);
    
    pinMode(2,INPUT_PULLUP);    //8 Bit pixil input from camera.
    pinMode(2, INPUT);          //
    pinMode(3,INPUT_PULLUP);    // 
    pinMode(3, INPUT);          //
    pinMode(4,INPUT_PULLUP);    //  
    pinMode(4, INPUT);          //
    pinMode(5,INPUT_PULLUP);    //  
    pinMode(5, INPUT);          //
    pinMode(6,INPUT_PULLUP);    //  
    pinMode(6, INPUT);          //
    pinMode(7,INPUT_PULLUP);    //  
    pinMode(7, INPUT);          //
    pinMode(8,INPUT_PULLUP);    //  
    pinMode(8, INPUT);          //
    pinMode(9,INPUT_PULLUP);    //   
    pinMode(9, INPUT);          //
  
    pinMode(VSYNC,INPUT_PULLUP);   //Frame strobe input or VSYNC
    pinMode(VSYNC, INPUT);
    

    //FIFO and Camera control inputs.
    pinMode(RRST, OUTPUT);       
    pinMode(WRST, OUTPUT);       
    pinMode(RCK, OUTPUT);        
    pinMode(WR, OUTPUT);        
  

    //LED pin (For debugging)
    pinMode(LED, OUTPUT);


Wire.setSDA(0);
Wire.setSCL(1);
Wire.begin();


//What you need to initiate I2C communication with the OV7670.

I2CwriteByte(CAMERA,REG_COM7,0b10000000);   //all registers default

I2CwriteByte(CAMERA,REG_CLKRC,0b10000000);  //double clock

I2CwriteByte(CAMERA,REG_COM11, 0b1000 | 0b10);   //enable auto 50/60Hz detect + exposure timing can be less...

//I2CwriteByte(CAMERA,REG_TSLB,0b100);     //sequence UYVY

I2CwriteByte(CAMERA,REG_TSLB,0x14);

I2CwriteByte(CAMERA,0x67, 0x80);
I2CwriteByte(CAMERA,0x68, 0x80);

//I2CwriteByte(CAMERA,REG_COM7, 0b100);    //RGB

//I2CwriteByte(CAMERA,REG_COM15, 0b11000000 | 0b010000); //RGB565

///////////////////////////////////////////////////////////////////////


I2CwriteByte(CAMERA,REG_COM10,0x02); //VSYNC negative

I2CwriteByte(CAMERA,REG_MVFP, 0x2b);  //mirror flip


//Test Image.
//I2CwriteByte(CAMERA,SCALING_YSC,0x35 | 0x80);  



//Image Scaler for 320x240 QVGA.
//I2CwriteByte(CAMERA,REG_COM14,0x19);
//I2CwriteByte(CAMERA,SCALING_DCWCTR,0x11);
//I2CwriteByte(CAMERA,SCALING_PCLK_DIV,0xf1);


//Image Scaler for 160x120 QQVGA.

//I2CwriteByte(CAMERA,REG_COM3, 0x04);
//I2CwriteByte(CAMERA,REG_COM14, 0x1a);
//I2CwriteByte(CAMERA,REG_SCALING_XSC, 0x3a);
//I2CwriteByte(CAMERA,REG_SCALING_YSC, 0x35);
//I2CwriteByte(CAMERA,REG_COM14,0x1a);
//I2CwriteByte(CAMERA,SCALING_DCWCTR,0x22);
//I2CwriteByte(CAMERA,SCALING_PCLK_DIV,0xf2);


//Image Scaler for 80x60 QQQVGA.

I2CwriteByte(CAMERA,REG_COM3, 0x04);
I2CwriteByte(CAMERA,REG_COM14, 0x1b);
I2CwriteByte(CAMERA,REG_SCALING_XSC, 0x3a);
I2CwriteByte(CAMERA,REG_SCALING_YSC, 0x35);
I2CwriteByte(CAMERA,REG_COM14,0x1b);
I2CwriteByte(CAMERA,SCALING_DCWCTR,0x33);
I2CwriteByte(CAMERA,SCALING_PCLK_DIV,0xf3);



//frameControl(196, 52, 8, 488);    // new lead



I2CwriteByte(CAMERA,REG_HSTART, 0x18);
I2CwriteByte(CAMERA,REG_HSTOP, 0x06);
I2CwriteByte(CAMERA,REG_HREF, 0x24);
I2CwriteByte(CAMERA,REG_VSTART, 0x02);
I2CwriteByte(CAMERA,REG_VSTOP, 0x7a);
I2CwriteByte(CAMERA,REG_VREF, 0x00);




I2CwriteByte(CAMERA,0xb0, 0x84);
saturation(10);
I2CwriteByte(CAMERA,0x13, 0xe7);
I2CwriteByte(CAMERA,0x6f, 0x9f);



//Experimental values for day time
//I2CwriteByte(CAMERA,0x11, 0x80);
//I2CwriteByte(CAMERA,0x6b, 0x0a);
//I2CwriteByte(CAMERA,0x2a, 0x00);
//I2CwriteByte(CAMERA,0x2b, 0x00);
//I2CwriteByte(CAMERA,0x92, 0x00);
//I2CwriteByte(CAMERA,0x93, 0x00);
//I2CwriteByte(CAMERA,0x3b, 0x0a);



//Values for day time
I2CwriteByte(CAMERA,0x11, 0x00);
I2CwriteByte(CAMERA,0x6b, 0x4a);
I2CwriteByte(CAMERA,0x2a, 0x00);
I2CwriteByte(CAMERA,0x2b, 0x00);
I2CwriteByte(CAMERA,0x92, 0x2b);
I2CwriteByte(CAMERA,0x93, 0x00);
I2CwriteByte(CAMERA,0x3b, 0x0a);


  


}


void readReset()
  {
      digitalWrite(RRST, 0);
      delayMicroseconds(1);
      digitalWrite(RCK, 0);
      delayMicroseconds(1);
      digitalWrite(RCK, 1);
      delayMicroseconds(1);
      digitalWrite(RRST, 1);  
  }

void writeEnable()
  {
      digitalWrite(WR, 0);
  }

void writeDisable()
  {
      digitalWrite(WR, 1);
  }

void writeReset()
  {
      digitalWrite(WRST, 0);
      delayMicroseconds(1);
      digitalWrite(WRST, 1);
  }






void loop() {

timeBegin = micros();
  
digitalWrite(LED, HIGH);

while(!bitRead(gpio_get_all(), VSYNC));
while(bitRead(gpio_get_all(), VSYNC));
   writeReset();
   writeEnable();
while(!bitRead(gpio_get_all(), VSYNC)); 
writeDisable();
Frame();




}

void Frame(){


  
 
  unsigned long timeEnd = micros();
 
 duration = timeEnd - timeBegin;


digitalWrite(LED, LOW);
   
  readReset();
   
    for (int i=0; i < 80; i++) {       

    for (int j=0; j < 60; j++) {   
          
    digitalWrite(RCK, LOW);
    digitalWrite(RCK, HIGH);

    //myImage[i][j] = ((gpio_get_all() >>2)  & (0b11111000));

    myImage[i][j] = gpio_get_all() >>2;
  
    digitalWrite(RCK, LOW);
    digitalWrite(RCK, HIGH);

           }

        }

   To_PC();  //To Processing

   //Timer_USB();       //For for testing frame rate
    //One_Byte_Of();    //For debugging single pixel 
}


void To_PC(){
                                     
  for(int y = 0; y < 80; y ++){                              
                                                                 
  for(int x = 0; x < 60; x ++){      
                                                                  
  Serial.print(myImage[y][x]);       
                                      
  Serial.print(",");                  
                                     
    }                                                                   
                                      
  }                                  
                                      
  Serial.println(",");               
                                      
    
}


void Timer_USB(){
  

  Serial.println(duration);

  
}


void One_Byte_Of(){

   Serial.println(myImage[40][30], BIN); 
  
}
