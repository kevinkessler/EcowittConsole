/**
 *  @filename   :   HumidityPanel.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console Humidity Display Class
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
#include "HumidityPanel.h"
#include "Adafruit_RA8875.h"
#include "display.h"
#include "InfluxDBQueries.h"

HumidityPanel::HumidityPanel(Adafruit_RA8875 *_tft, uint16_t _x, uint16_t _y, int8_t _current, bool _indoor) {
  tft = _tft;
  x_org = _x;
  y_org = _y;
  humidity = _current;
  dewPoint = 70;
  low = _current;
  high = _current;
  indoor = _indoor;

  highlow=DAILY;
  hasData = false;

  displayMode = HUM_MODE;
  humDirty = true;
  borderDirty = true;
  extremeDirty = true;
}

void HumidityPanel::draw() {

  if(borderDirty) {
    tft->graphicsMode();
    tft->drawLine(x_org,y_org, x_org,y_org+HUM_HEIGTH-25,RA8875_YELLOW); // Left
    tft->drawLine(x_org+HUM_WIDTH,y_org, x_org+HUM_WIDTH,y_org+HUM_HEIGTH-25,RA8875_YELLOW); // Right
    tft->drawLine(x_org+25,y_org+HUM_HEIGTH, x_org+HUM_WIDTH-25,y_org+HUM_HEIGTH,RA8875_YELLOW); //Bottom
    tft->drawCurve(x_org+25, y_org+HUM_HEIGTH-25, 25, 25,0,RA8875_YELLOW);
    tft->drawCurve(x_org+HUM_WIDTH-25, y_org+HUM_HEIGTH-25, 25, 25,3,RA8875_YELLOW);

    redrawBackgroundSection(x_org + 3, y_org +3, HUM_WIDTH - 25, 20);
    tft->textMode();
    tft->textTransparent(RA8875_WHITE);    
    tft->textEnlarge(0);


    if (indoor) {
      tft->textSetCursor(x_org+(HUM_WIDTH-154)/2, y_org+3);
      printString("Indoor Humidity");
    }
    else{
      tft->textSetCursor(x_org+(HUM_WIDTH-160)/2, y_org+3);
      printString("Outdoor ");
      if(displayMode == HUM_MODE)
        printString("Humidity");
      else 
        printString("Dew Point");
    }
    
    borderDirty = false;
  }

  if(humDirty) {
    uint16_t xoffset = 60;
    uint16_t yoffset = 30;   
    
    redrawBackgroundSection(x_org + xoffset - 20, y_org + yoffset, HUM_WIDTH - 75, 75);
    setArialFont();
    tft->textEnlarge(1);
    

    if(displayMode == HUM_MODE) {
      if(abs(humidity) < 10) {
        xoffset+=38;
      }

      
      tft->textSetCursor(x_org + xoffset, y_org + yoffset);

      char buffer[5];
      itoa(humidity,buffer,10);
      printString(buffer);
      printString("%");

      humDirty = false;
    } else {  // Dew Point Display
  
      xoffset = 90;

      if(abs(dewPoint) < 10) {
        xoffset+=38;
      }

      
      tft->textSetCursor(x_org + xoffset, y_org + yoffset);

      char buffer[5];
      itoa(dewPoint,buffer,10);
      printString(buffer);

      humDirty = false;
    }

  }
  
  if(extremeDirty) {
    drawExtremes();
    extremeDirty = false;
  }
}

void HumidityPanel::drawExtremes() {

  redrawBackgroundSection(x_org+ 1, y_org+HUM_XTREME_YOFFSET, HUM_WIDTH-1, 49);

  tft->textMode();
  tft->textTransparent(RA8875_WHITE);
  
  tft->textEnlarge(0);

  tft->textSetCursor(x_org+25,y_org+HUM_XTREME_YOFFSET);
  printString("Low");
  tft->textSetCursor(x_org+HUM_WIDTH-27-32,y_org+HUM_XTREME_YOFFSET);
  printString("High");

  //Character Width = 8 Space =2
  setSmallArialFont();
  switch(highlow) {
    case DAILY:
      getDailyExtremes();
      tft->textSetCursor(x_org+(HUM_WIDTH/2)-24,y_org+HUM_XTREME_YOFFSET+15);
      printString("Daily");
      break;
    case WEEKLY:
      getExtendedExtremes(7);
      tft->textSetCursor(x_org+(HUM_WIDTH/2)-29,y_org+HUM_XTREME_YOFFSET+15);
      printString("Weekly");
      break;
    case MONTHLY:
      getExtendedExtremes(30);
      tft->textSetCursor(x_org+(HUM_WIDTH/2)-34,y_org+HUM_XTREME_YOFFSET+15);
      printString("Monthly");
      break;
    case YEARLY:
      getExtendedExtremes(365);
      tft->textSetCursor(x_org+(HUM_WIDTH/2)-29,y_org+HUM_XTREME_YOFFSET+15);
      printString("Yearly");
      break;      
    default:
      break;
  }
  
  drawCenteredArial((3*8)/2+25+x_org,y_org+HUM_XTREME_YOFFSET+15,low);
  drawCenteredArial(x_org+(HUM_WIDTH -27 -(4*8)/2),y_org+HUM_XTREME_YOFFSET+15,high);
}

bool HumidityPanel::isClicked(uint16_t x, uint16_t y) {

  if((x>(x_org+HUM_CLICK_MIN_X))&&(y>(y_org+HUM_CLICK_MIN_Y))&&(x<(x_org+HUM_CLICK_MAX_X))&&(y<(y_org+HUM_XTREME_YOFFSET+HUM_CLICK_MAX_Y))) {
    

    if((y<(y_org+HUM_XTREME_YOFFSET))&&(indoor == false)) {

      if (displayMode == HUM_MODE)
        displayMode = DP_MODE;
      else
        displayMode = HUM_MODE;
      humDirty=true;
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

void HumidityPanel::setHumidity(uint8_t _humidity) {
  hasData=true;

  if (humidity == _humidity) {
    return;
  }

  humidity = _humidity;
  if(humidity > 99) {
    humidity = 99;
  }

  humDirty = true;
  if(humidity < low) {
    extremeDirty = true;
    low = humidity;
  }

  if(humidity > high) {
    extremeDirty = true;
    high = humidity;
  }

}

void HumidityPanel::setDewPoint(uint8_t _dewPoint) {

  if (dewPoint == _dewPoint) {
    return;
  }

  dewPoint = _dewPoint;

  humDirty = true;
  if(dewPoint < lowDew) {
    extremeDewDirty = true;
    lowDew = dewPoint;
  }

  if(dewPoint > highDew) {
    extremeDewDirty = true;
    highDew = dewPoint;
  }

}

void HumidityPanel::getDailyExtremes() {

  if(!hasData)
    return;

  float newHigh,newLow;

  newHigh = 65;
  newLow = 45;

  if(influxGetHighLowHum(indoor, 1, &newHigh, &newLow))
    return;       //Some error occured

  // Round Up
  newHigh += 0.5;
  newLow += 0.5;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}

void HumidityPanel::getExtendedExtremes(uint16_t timeLen) {
  float newHigh, newLow;
  if(!hasData)
    return;

  if(influxGetHighLowHum(indoor, timeLen, &newHigh, &newLow))
    return;       //Some error occured
  
  if(high > newHigh)
    newHigh=high;

  if(low < newLow)
    newLow = low;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}