/**
 *  @filename   :   TemperaturePanel.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console Temperature Panel Class
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
#include "TemperaturePanel.h"
#include "Adafruit_RA8875.h"
#include "display.h"
#include "InfluxDBQueries.h"


TemperaturePanel::TemperaturePanel(Adafruit_RA8875 *_tft, uint16_t _x, uint16_t _y, float _current, bool _indoor) {

  tft = _tft;
  x_org = _x;
  y_org = _y;
  temperature = _current;
  feels_like = _current;
  high=-125;
  low=125;

  indoor = _indoor;  
  
  //getDailyExtremes();
  refreshCount=0;
  displayMode=TEMP_MODE;

  highlow=DAILY;

  tempDirty = true;
  borderDirty = true;
  extremeDirty = true;
  hasData=false;
  
}

void TemperaturePanel::setTemperature(float _temperature) {
  hasData=true;

  int rndTemp = int(temperature * 10);
  int rndNewTemp = int(_temperature *10);

  if(rndTemp == rndNewTemp) {
    temperature = _temperature;
    return;
  }

  temperature = _temperature;
  tempDirty = true;

  if (temperature < -99)
    temperature = -99;

 
  if(temperature < low) {
    extremeDirty = true;
    low = temperature;
  }

  if(temperature > high) {
    extremeDirty = true;
    high = temperature;
  }

}

void TemperaturePanel::setFeelsLike(float _feels_like) {
  hasData=true;
  int rndFeel = int(feels_like* 10);
  int rndNewFeel = int(_feels_like *10);

  if(rndFeel == rndNewFeel) {
    feels_like = _feels_like;
    return;
  }

  feels_like = _feels_like;
  tempDirty = true;

  if (feels_like < -99)
    feels_like = -99;

  if(feels_like < low) {
    extremeDirty = true;
    lowFeels = feels_like;
  }

  if(feels_like > high) {
    extremeDirty = true;
    highFeels = feels_like;
  }

}

void TemperaturePanel::draw() {

  if(borderDirty) {
    tft->graphicsMode();
    tft->drawLine(x_org+25,y_org, x_org+TEMP_WIDTH-25,y_org,RA8875_YELLOW); // Top
    tft->drawLine(x_org,y_org+25, x_org,y_org+TEMP_HEIGTH,RA8875_YELLOW); // Left
    tft->drawLine(x_org+TEMP_WIDTH,y_org+25, x_org+TEMP_WIDTH,y_org+TEMP_HEIGTH,RA8875_YELLOW); // Right
    tft->drawLine(x_org,y_org+TEMP_HEIGTH, x_org+TEMP_WIDTH,y_org+TEMP_HEIGTH,RA8875_YELLOW); //Bottom
    tft->drawCurve(x_org+25, y_org+25, 25, 25,1,RA8875_YELLOW);
    tft->drawCurve(x_org+TEMP_WIDTH-25, y_org+25, 25, 25,2,RA8875_YELLOW);

    redrawBackgroundSection(x_org + 14, y_org +3, TEMP_WIDTH - 28, 20);
    tft->textMode();
    tft->textTransparent(RA8875_WHITE);
    tft->textEnlarge(0);


    if (indoor) {
      tft->textSetCursor(x_org+(TEMP_WIDTH-172)/2, y_org+3);
      printString("Indoor Temperature");
    }
    else{
      
      if(displayMode == TEMP_MODE) {
        tft->textSetCursor(x_org+(TEMP_WIDTH-167)/2, y_org+3);
        printString("Outdoor Temperature");
      } else { 
        tft->textSetCursor(x_org+(TEMP_WIDTH-212)/2, y_org+3);
        printString("Feels-Like Temperature");
      }
    }
    borderDirty = false;
  }

  if(tempDirty) {
    redrawBackgroundSection(x_org + 20, y_org + 40, TEMP_WIDTH - 50, 90);

    setArialFont();
    tft->textEnlarge(1);

    uint16_t xoffset = 60;
    uint16_t yoffset = 60;

    if (displayMode==TEMP_MODE) {
      if(abs(temperature) < 10) {
        xoffset+=15;
      } else if(abs(temperature)> 99) {
        xoffset-=25;
      }

      if(temperature < 0) 
        xoffset-=25;
      
      tft->textSetCursor(x_org + xoffset, y_org + yoffset);

      char buffer[7];
      sprintf(buffer,"%3.1f", temperature);
      printString(buffer);

      //drawThermometer(x_org+180,y_org+45);
      tempDirty = false;
    } else {
      if(abs(feels_like) < 10) {
        xoffset+=48;
      } else if(abs(feels_like)> 99) {
        xoffset-=32;
      }

      if(feels_like < 0) 
        xoffset-=32;
      
      tft->textSetCursor(x_org + xoffset, y_org + yoffset);

      char buffer[7];
      sprintf(buffer,"%3.1f", feels_like);
      printString(buffer);

      //drawThermometer(x_org+180,y_org+45);
      tempDirty = false;
    }

  }

  if(extremeDirty) {
    drawExtremes();
    extremeDirty = false;
  }

  
}

void TemperaturePanel::drawExtremes() {
  redrawBackgroundSection(x_org+1, y_org+TEMP_XTREME_YOFFSET, TEMP_WIDTH-1, 49);

  tft->textMode();
  tft->textTransparent(RA8875_WHITE);
  
  tft->textEnlarge(0);

  tft->textSetCursor(x_org+25,y_org+TEMP_XTREME_YOFFSET);
  printString("Low");
  tft->textSetCursor(x_org+TEMP_WIDTH-27-32,y_org+TEMP_XTREME_YOFFSET);
  printString("High");

  //Character Width = 8 Space =2
  setSmallArialFont();
  switch(highlow) {
    case DAILY:
      getDailyExtremes();
      tft->textSetCursor(x_org+(TEMP_WIDTH/2)-24,y_org+TEMP_XTREME_YOFFSET+15);
      printString("Daily");
      break;
    case WEEKLY:
      getExtendedExtremes(7);
      tft->textSetCursor(x_org+(TEMP_WIDTH/2)-29,y_org+TEMP_XTREME_YOFFSET+15);
      printString("Weekly");
      break;
    case MONTHLY:
      getExtendedExtremes(30);
      tft->textSetCursor(x_org+(TEMP_WIDTH/2)-34,y_org+TEMP_XTREME_YOFFSET+15);
      printString("Monthly");
      break;
    case YEARLY:
      getExtendedExtremes(365);
      tft->textSetCursor(x_org+(TEMP_WIDTH/2)-29,y_org+TEMP_XTREME_YOFFSET+15);
      printString("Yearly");
      break;      
    default:
      break;
  }
  
  drawCenteredArial((3*8)/2+25+x_org,y_org+TEMP_XTREME_YOFFSET+15,low);
  drawCenteredArial(x_org+(TEMP_WIDTH -27 -(4*8)/2),y_org+TEMP_XTREME_YOFFSET+15,high);
}

void TemperaturePanel::getDailyExtremes() {
  float newHigh,newLow;
  if(!hasData)
    return;

  if(influxGetHighLowTemp(indoor, 1, &newHigh, &newLow))
    return;       //Some error occured

  // Round Up
  newHigh += 0.5;
  newLow += 0.5;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}

void TemperaturePanel::getExtendedExtremes(uint16_t timeLen) {
  float newHigh, newLow;
  if(!hasData)
    return;

  if(influxGetHighLowTemp(indoor, timeLen, &newHigh, &newLow))
    return;
  
  if(high > newHigh)
    newHigh=high;

  if(low < newLow)
    newLow = low;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}

bool TemperaturePanel::isClicked(uint16_t x, uint16_t y) {

  if((x>(x_org+TEMP_CLICK_MIN_X))&&(y>(y_org+TEMP_CLICK_MIN_Y))&&(x<(x_org+TEMP_CLICK_MAX_X))&&(y<(y_org+TEMP_XTREME_YOFFSET+TEMP_CLICK_MAX_Y))) {
    if((y<(y_org+TEMP_XTREME_YOFFSET))&&(indoor == false)) {
      if (displayMode == TEMP_MODE)
        displayMode = FEELS_MODE;
      else
        displayMode = TEMP_MODE;
      tempDirty=true;
      borderDirty=true;
    } else {
      extremeDirty = true;
      switch(highlow) {
        case DAILY:
          highlow=WEEKLY;
          break;
        case WEEKLY:
          highlow=MONTHLY;
          break;
        case MONTHLY:
          highlow=YEARLY;
          break;
        default:
          highlow=DAILY;
          break;
      }
    }

    draw();
    return true;
  }

  return false;
}


