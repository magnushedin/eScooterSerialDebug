#include <U8g2lib.h>
#include <Wire.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

byte prevByte = 0;
byte thisByte = 0;
byte inByte[11] = {0xAA,0x0A,0x8B,0xF1,0x01,0x0B,0x0B,0x55,0x66};
int cnt = 0;
int pos = 0;
unsigned int velocity = 0;
unsigned int battVolt = 0;

void setup() {
  Serial.begin(9600);
  while(!Serial) {}
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_5x7);
  u8g2.drawStr(5, 20, "Waiting for connection");
  u8g2.sendBuffer();

  // Wait for serial buss to start
  while (!Serial.available()) {}
  delay(1000);
}

void loop() {
  // Read serial message
  while(Serial.available() > 0) {
    prevByte = thisByte;
    thisByte= Serial.read();
    if (prevByte == 0xFF && thisByte == 0xAA) {
      cnt = 0;
    }
    inByte[cnt] = thisByte;
    cnt++;
    if (cnt > 10) {
      cnt = 10;
    }
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_5x7);

  // Serial message DEC
  pos = 0;
  for (int i = 1; i<8; i++) {
    u8g2.setCursor(pos,6);
    u8g2.print(inByte[i], DEC);
    pos += 17;
    //Serial.print(inByte[i]);
  }
  
  // Serial message HEX
  pos = 0;
  for (int i = 1; i<8; i++) {
    u8g2.setCursor(pos,15);
    u8g2.print(inByte[i], HEX);
    pos += 17;
    //Serial.print(inByte[i]);
  }

  // Debug
  u8g2.setFont(u8g_font_10x20);

  // Power?
  u8g2.setCursor(0,50);
  u8g2.print(inByte[1], HEX);
  u8g2.setCursor(25,50);
  u8g2.print(inByte[1], DEC);
  
  // Break?
  u8g2.setCursor(0,64);
  u8g2.print(inByte[6], HEX);
  u8g2.setCursor(25,64);
  u8g2.print(inByte[6], DEC);

  // Batt volt
  battVolt = inByte[2] << 8;
  battVolt += ((inByte[3]) & 0x00FF);
  u8g2.setFont(u8g_font_10x20);
  u8g2.setCursor(80, 50);
  u8g2.print(((float)battVolt)/1000, 1);
  
  // Velocity
  velocity = inByte[4] << 8;
  velocity += (inByte[5]) & 0x00FF;
  u8g2.setFont(u8g_font_10x20);
  u8g2.setCursor(80,64);
  //u8g2.print(((float)((velocity*60)*(0.225*3.14)))/10, 1);
  u8g2.print(((float)(map(velocity,0,1024,0,440))/10), 1);

  // Send to screen
  u8g2.sendBuffer();
}
