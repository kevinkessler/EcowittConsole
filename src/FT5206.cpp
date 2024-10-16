/**
 *  @filename   :   FT5206.cpp
 *  @brief      :   ESP32 Weather Base Station Touchscreen module
 *
 *  @author     :   Kevin Kessler
 *
 * Copyright (C) 2024 Kevin Kessler
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */ 

#include <Arduino.h>
#include "Wire.h"
#include "FT5206.h"
#include "display.h"



uint32_t previousTouchTime=0;
volatile bool touchInt = false;

struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

static uint8_t readFT5206TouchRegister( uint8_t reg )
{
  Wire.beginTransmission(FT5206_ADDR);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  Wire.requestFrom(FT5206_ADDR, int(1) );    // request 6 uint8_ts from slave device #2
  
  if (Wire.available())
  {
    retVal = Wire.read();
  }
  
  return retVal;
}

static uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(FT5206_ADDR);
  Wire.write( regAddr );  // register 0
  Wire.endTransmission();
  
  Wire.requestFrom(FT5206_ADDR, (int)len);    // request 1 bytes from slave device #2
  
  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }
  
  return i;
}

static void writeFT5206TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(FT5206_ADDR);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value
  
  Wire.endTransmission();  
}

static uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  
  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one
    
    uint8_t status = readFT5206TouchRegister(2);
    
    static uint8_t tbuf[40];
    
    if ((status & 0x0f) == 0) break; // no points detected
    
    uint8_t hitPoints = status & 0x0f;

    
    readFT5206TouchAddr( 0x03, tbuf, hitPoints*6);
    
    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }
    
    retVal = k;
    
  } while (0);
  
  return retVal;
}

static void ft5206_reset() {
  digitalWrite(FT5206_RST_PIN,LOW);
  delay(10);
  digitalWrite(FT5206_RST_PIN,HIGH);
}

void ft5206_init() {
  pinMode(FT5206_RST_PIN,OUTPUT);
  pinMode(FT5206_WAKE_PIN,INPUT);
  pinMode(FT5206_INT_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(FT5206_INT_PIN),ft5206_interrupt,FALLING);
  ft5206_reset();
  Wire.begin();
    
  digitalWrite(FT5206_WAKE_PIN,HIGH);
  delay(10);
  writeFT5206TouchRegister(0,CTP_NORMAL);

}

void processTouchScreen() {

  uint32_t touchTime = millis();
  if(touchTime - previousTouchTime> 50) {
    uint8_t count = readFT5206TouchLocation(touchLocations, 1);

// Sometime render time of panel takes longer than 50ms, which can cause multiple triggers for each touch
// Time between triggers (50ms start time) must be set after the render
    tftCTPTouch(touchLocations[0].x, touchLocations[0].y);  

  }
  
  previousTouchTime = millis();
  touchInt = false;

}

void checkTouch() {
  if(touchInt)
    processTouchScreen();
}

ICACHE_RAM_ATTR void ft5206_interrupt() {
  touchInt = true;
} 