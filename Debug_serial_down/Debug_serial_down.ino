#include <U8g2lib.h>
#include <Wire.h>
#include <TimerOne.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define BUTTON_PIN 2
#define THROTTLE_PIN A0
#define BREAK_BIT 0x20

byte prevByte = 0;
byte thisByte = 0;
byte inByte[9] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
byte outByte[9] = { 0xFF, 0xAA, 0x00, 0x32, 0x01, 0x00, 0x00, 0x03, 0x00 };
int cnt = 0;
int pos = 0;
int throttleValue = 0;
byte throttleValue_byte = 0x00; 

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(THROTTLE_PIN, INPUT);
  Timer1.initialize(100000);
  Timer1.attachInterrupt(serialSender);
  Serial.begin(9600);
  while(!Serial) {}
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_5x7);
  u8g2.drawStr(5, 20, "Waiting for connection");
  u8g2.sendBuffer();

  // Wait for serial buss to start
  //while (!Serial.available()) {}
  delay(1000);
}

void serialSender()
{
  // Init serial output
  //outByte[3] = 0x00; // Throttle
  outByte[7] = 0x03; // Data
  outByte[8] = 0x00;
  
  // Create serial output
  if (digitalRead(BUTTON_PIN) == LOW)
  {
    outByte[7] = outByte[7] | BREAK_BIT;
  }

  throttleValue = analogRead(THROTTLE_PIN);
  throttleValue_byte = map(throttleValue, 0, 1023, 5, 290);

  outByte[3] = throttleValue_byte;

  // Checksum
  for (int i = 0; i < 8; i++)
  {
    outByte[8] += outByte[i];
  }

  for (int i = 0; i < 9; i++)
  {
    Serial.write(outByte[i]);
    }
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

  // My own serial message to be sent
  pos = 0;
  for (int i = 2; i<9; i++) {
    u8g2.setCursor(pos,6);
    u8g2.print(outByte[i], HEX);
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

  // TPS?
  u8g2.setCursor(0,50);
  u8g2.print(inByte[2], HEX);
  u8g2.setCursor(25,50);
  u8g2.print(inByte[2], DEC);

  // eco
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(0, 29);
  if (inByte[6] & 0x01) {
    u8g2.print("std");
  }
  else {
    u8g2.print("eco");
  }

  // Light
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(30, 29);
  if (inByte[6] & 0x80)
  {
    u8g2.print("Light on");
  }
  else
  {
    u8g2.print("Light off");
  }

  // Break
  u8g2.setFont(u8g_font_5x7);
  u8g2.setCursor(80, 29);
  if (inByte[6] & 0x20)
  {
    u8g2.print("breaking");
  }


  // Break?
  /*
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
  */

  // Send to screen
  u8g2.sendBuffer();
}
