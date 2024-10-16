/**
 *  @filename   :   display.cpp
 *  @brief      :   Ecowitt Weather Station Console, display module
 *  @author     :   Kevin Kessler
 *
 * Copyright (C) 2024 Kevin Kessler
 *  
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include <SPI.h>
#include <time.h>
#include "Ticker.h"
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "Adafruit_I2CDevice.h"
#include "display.h"
#include "PanelBase.h"
#include "TemperaturePanel.h"
#include "HumidityPanel.h"
#include "HeaderPanel.h"
#include "ErrorPanel.h"
#include "RainPanel.h"
#include "BaroPanel.h"
#include "WindPanel.h"
#include "FT5206.h"

Adafruit_RA8875 tft = Adafruit_RA8875(CS, RST);

PanelList *first = NULL;
TemperaturePanel *tp1, *tp2;
HumidityPanel *hp1, *hp2;
HeaderPanel *headp;
ErrorPanel *ep;
RainPanel *rp;
BaroPanel *bp;
WindPanel *wp;

void resetTickerCallback(void);
void dataTickerCallback(void);
Ticker dataTimer(dataTickerCallback,300000); // 5 minutes
Ticker resetTimer(resetTickerCallback,300000);

static void waitForSignal(){
  uint16_t count=0;
  while(digitalRead(WAIT_PIN)!=1) {
    if(++count==0)          // Timeout of ~9ms, normally this takes ~720uS for large fonts
      break;
  }
}

void drawCenteredArial(uint16_t centerx, uint16_t centery, int8_t value) {
  setArialFont();
  tft.textEnlarge(0);

  uint8_t textLength=16;
  if(abs(value)>99)
    textLength+=20;
  if(abs(value) > 9)
    textLength+=20;

  int16_t startx=centerx - textLength/2;
  if(value<0)                     // Space for the negitive sign
    startx-=13;
    if(startx<0)
      startx=0;  
  
  char buffer[5];
  tft.textSetCursor(startx,centery);
  itoa(value,buffer,10);
  printString(buffer);

}

void setError(const char *errStr) {
  if(ep == NULL)
    return;

  ep->setMessage(errStr);
  //log("errorpanel", errStr);
}

void clearError() {
  if(ep == NULL)
    return;

  ep->clearMessage();
}

void setArialFont(){
  tft.textMode();          // Resets font info, so don't run this after setting font
  tft.writeReg(0x21,0x20); // Font Control Register, turn on external CGROM, bit 5
  tft.writeReg(0x06,0x00); // Serial Flash CLK, SFCL=SystemClock
  tft.writeReg(0x2e,0x82); // Font Write Type, 80=32x32, 5-0=font to font pixels
  tft.writeReg(0x2f,0x91); // Serial Font Select, 80=GB2312 90=ASCII 8C=Unicode 84=GB12345 
  tft.writeReg(0x29,0x05); // Font line spacing
  tft.writeReg(0x05,0x28); // Serial Flash Rom Config
}

void setSmallArialFont(){
  tft.textMode();          // Resets font info, so don't run this after setting font
  tft.writeReg(0x21,0x20); // Font Control Register, turn on external CGROM, bit 5
  tft.writeReg(0x06,0x00); // Serial Flash CLK, SFCL=SystemClock
  tft.writeReg(0x2e,0x00); // Font Write Type, 80=32x32, 5-0=font to font pixels
  tft.writeReg(0x2f,0x91); // Serial Font Select, 80=GB2312 90=ASCII 8C=Unicode 84=GB12345 
  tft.writeReg(0x29,0x05); // Font line spacing
  tft.writeReg(0x05,0x28); // Serial Flash Rom Config
}

void printString(const char *s) {

  tft.writeCommand(RA8875_MRWC);
  while(*s!=0) {
      tft.writeData(*s);
      ++s;
      waitForSignal();
  }
}

void redrawBackgroundSection(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

  if((h*w*2) > 200000) {
    Serial.println("Memory Allocation too large in redrawBackgroundSection");
    return;
  }

  uint8_t *bitmap=(uint8_t*)malloc(h*w*2);
  
  for(uint16_t n=0;n<h;n++) {
    memcpy(&bitmap[w*n*2],&background_bmp[((y+n)*800+x)*2],w*2);
  }

  drawTransparentBitmap(x,y,w,h,bitmap); 
  
  free(bitmap);
}

void display_panels() {
  ep = new ErrorPanel(&tft);
  ep->draw();

  if(first==NULL) {
    first = (PanelList *)malloc(sizeof(PanelList));
  }

  //log("temperature","Temp 1 Create");
  tp1 = new TemperaturePanel(&tft, 0, 30, 75.0,false);
  tp1->draw();

  //log("temperature","Temp 2 Create");
  tp2 = new TemperaturePanel(&tft, 549, 30, 75.0,true);
  tp2->draw();

  //log("temperature","Hum 1 Create");
  hp1 = new HumidityPanel(&tft,0,261,50,false);
  hp1->draw();

  hp2 = new HumidityPanel(&tft,549,261,50,true);
  hp2->draw();

  rp = new RainPanel(&tft,255,30);
  rp -> draw();

  bp = new BaroPanel(&tft,255,160);
  bp->draw();

  wp = new WindPanel(&tft,255,330);
  wp->draw();

  headp = new HeaderPanel(&tft);
  headp->draw();

  first->p = tp1;
  PanelList *next = (PanelList *)malloc(sizeof(PanelList));
  first->next = next;

  next->p = tp2;
  PanelList *hnext = (PanelList *)malloc(sizeof(PanelList));
  next->next=hnext;
  hnext->p = hp1;

  next = (PanelList *)malloc(sizeof(PanelList));
  hnext->next = next;
  next->p = hp2;

  hnext = (PanelList *)malloc(sizeof(PanelList));
  next->next = hnext;
  hnext->p = rp;

  next = (PanelList *)malloc(sizeof(PanelList));
  hnext->next = next;
  next->p = bp;

  hnext = (PanelList *)malloc(sizeof(PanelList));  
  next->next=hnext;
  hnext->p = wp;

  next = (PanelList *)malloc(sizeof(PanelList));
  hnext->next = next;
  next->p = headp; 
  next->next=NULL;


}

void drawAll() {
  dataTimer.stop();
  dataTimer.start();

  if (first==NULL)
    return;

  PanelList *p = first;
  while(p!=NULL) {
    p->p->draw();
    p = p->next;
  }  

}

void conversionError(const char *column, char *value) {
  char error[50];

  sprintf(error,"Bad conversion for %s %s", column, value);
  setError(error);
}

bool setData(char *name, char *value) {

  if(strcmp("drain_piezo", name)==0) {
    char *ending;
    float rain = strtof(value,&ending);
    if(*ending==0) {
      rp->setDailyRain(rain);
      return true;
    } else { 
      conversionError("drain_piezo",value);
      return false;
    }
  }

  if(strcmp("wrain_piezo", name)==0) {
    char *ending;
    float rain = strtof(value,&ending);
    if(*ending==0) {
      rp->setWeeklyRain(rain);
      return true;
    } else {
      conversionError("wrain_piezo",value);
      return false;
    }
  }

    if(strcmp("mrain_piezo", name)==0) {
    char *ending;
    float rain = strtof(value,&ending);
    if(*ending==0) {
      rp->setMonthlyRain(rain);
      return true;
    } else { 
      conversionError("mrain_piezo",value);

      return false;
    }
  }

  if(strcmp("yrain_piezo", name)==0) {
    char *ending;
    float rain = strtof(value,&ending);
    if(*ending==0) {
      rp->setYearlyRain(rain);
      return true;
    } else { 
      conversionError("yrain_piezo",value);
      return false;
    }
  }

  if(strcmp("humidity", name)==0) {
    char *ending;
    float humidity = strtof(value,&ending);
    if(*ending==0) {
      hp1->setHumidity(humidity);
      return true;
    } else { 
      conversionError("humidity",value);
      return false;
    }
  }

  if(strcmp("dewpoint", name)==0) {
    char *ending;
    float dew = strtof(value,&ending);
    if(*ending==0) {
      hp1->setDewPoint(dew);
      return true;
    } else { 
      conversionError("dew_point",value);
      return false;
    }
  }

  if(strcmp("humidityin", name)==0) {
    char *ending;
    float humidity = strtof(value,&ending);
    if(*ending==0) {
      hp2->setHumidity(humidity);
      return true;
    } else { 
      conversionError("humidityin",value);
      return false;
    }
  }

  if(strcmp("temp", name)==0) {
    char *ending;
    float temp= strtof(value,&ending);
    if(*ending==0) {
      tp1->setTemperature(temp);
      return true;
    } else { 
      conversionError("temp",value);
      return false;
    }
  }

    if(strcmp("feelslike", name)==0) {
    char *ending;
    float feels = strtof(value,&ending);
    if(*ending==0) {
      tp1->setFeelsLike(feels);
      return true;
    } else { 
      conversionError("feelslike",value);
      return false;
    }
  }

  if(strcmp("tempin", name)==0) {
    char *ending;
    float temp = strtof(value,&ending);
    if(*ending==0) {
      tp2->setTemperature(temp);
      return true;
    } else { 
      conversionError("tempin",value);
      return false;
    }
  }

  if(strcmp("windspeed", name)==0) {
    char *ending;
    float wind = strtof(value,&ending);
    if(*ending==0) {
      wp->setWind(wind);
      return true;
    } else { 
      conversionError("windspeed",value);
      return false;
    }
  }

  if(strcmp("windgust", name)==0) {
    char *ending;
    float gust = strtof(value,&ending);
    if(*ending==0) {
      wp->setGust(gust);
      return true;
    } else { 
      conversionError("windgust",value);
      return false;
    }
  }

  if(strcmp("maxdailygust", name)==0) {
    char *ending;
    float maxGust = strtof(value,&ending);
    if(*ending==0) {
      wp->setMaxGust(maxGust);
      return true;
    } else { 
      conversionError("maxdailygust",value);
      return false;
    }
  }

  if(strcmp("winddir_name",name)==0) {
    wp->setDirection(value);
    return true;
  }

  if(strcmp("wh90batt", name)==0) {
    char *ending;
    float batt = strtof(value,&ending);
    if(*ending==0) {
      headp->setBatteryLevel(batt);
      return true;
    } else { 
      conversionError("wh90batt",value);
      return false;
    }
  }

  if(strcmp("baromrel", name)==0) {
    char *ending;
    float baro = strtof(value,&ending);
    if(*ending==0) {
      bp->setPressure(baro);
      return true;
    } else { 
      conversionError("baromrel",value);
      return false;
    }
  }

  return false;
}

void drawTransparentBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *bitmap) {

  tft.writeReg(0x58,x & 0xff);
  tft.writeReg(0x59,(x>>8));
  tft.writeReg(0x5A,y & 0xff);
  tft.writeReg(0x5B,(y>>8));
  tft.writeReg(0x5C,w & 0xff);
  tft.writeReg(0x5D, (w>>8));  
  tft.writeReg(0x5E,h & 0xff);
  tft.writeReg(0x5F,(h>>8));

  // White is the transparent color
  tft.writeReg(0x63,0xff);
  tft.writeReg(0x64,0xff);
  tft.writeReg(0x65,0xff); 

  tft.writeReg(0x51,0xc4);
  uint8_t reg=tft.readReg(0x50);
  reg=reg|0x80;
  tft.writeReg(0x50,reg);

  tft.writeCommand(RA8875_MRWC);


  digitalWrite(CS,LOW);

  SPI.beginTransaction(SPISettings(20000000UL, MSBFIRST, SPI_MODE0));
  SPI.transfer(RA8875_DATAWRITE);
   
  SPI.writeBytes(bitmap,h*w*2);   

  SPI.endTransaction();
  digitalWrite(CS,HIGH);

}

void background_panel() {
  drawTransparentBitmap(0,0,800,480,background_bmp);
}

void tftCTPTouch(uint16_t x, uint16_t y) {
  resetTimer.stop(); // reset timer that puts everything back to Daily extremes after 5 minutes
  resetTimer.start();

  if (first==NULL)
    return;

  PanelList *p = first;
  while(p!=NULL) {
    if(p->p->isClicked(x,y))
      break;
    p = p->next;
  }  

}
void displayLoop(void) {

  checkTouch();

  dataTimer.update();
  resetTimer.update();
}

void resetTickerCallback() {
  Serial.println("!!!!!Reset Timer!!!!!!!");
  clearError();

}

void dataTickerCallback() {
  Serial.println("!!!!!!!!Data Timeout!!!!!!!!!!");
  ep->setMessage("Error: No Data from station in 5 Minutes");
}

void initDisplay() {
  Serial.println("Display Start");
  if(!tft.begin(RA8875_800x480)) {
    Serial.println("Failed");
    return;
  }

  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true,RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(128);

  tft.fillScreen(RA8875_BLACK);

  pinMode(WAIT_PIN,INPUT);
  ft5206_init();

  background_panel();
  display_panels();
  dataTimer.start();
  resetTimer.start();
}