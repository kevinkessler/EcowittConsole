/**
 *  @filename   :   BaroPanel.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console Barometer Display Class
 *
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
#include "BaroPanel.h"
#include "Adafruit_RA8875.h"
#include "display.h"
#include "InfluxDBQueries.h"

BaroPanel::BaroPanel(Adafruit_RA8875 *_tft, uint16_t _x, uint16_t _y) {
  tft = _tft;
  x_org = _x;
  y_org = _y;
  pressure = 900.0;
  low = 0.0;
  high = pressure;

  baroDir = BARO_STEADY;
  averagePoll=0;
  average = 0.0;

  displayMode = HPA_MODE;
  highlow=DAILY;

  baroDirty = true;
  borderDirty = true;
  extremeDirty = true;
}

void BaroPanel::draw() {

  if(borderDirty) {
    tft->graphicsMode();
    tft->drawRect(x_org,y_org,BARO_WIDTH+1,BARO_HEIGTH,RA8875_YELLOW);

    tft->textMode();
    tft->textTransparent(RA8875_WHITE);
    tft->textEnlarge(0);
    tft->textSetCursor(x_org+(BARO_WIDTH - 130)/2, y_org+3);

    redrawBackgroundSection(x_org + 14, y_org +3, BARO_WIDTH - 28, 20);
    if(displayMode==HPA_MODE)
      printString("Barometer hPa");
    else 
      printString("Barameter inHg");

    borderDirty = false;
  }

  if(baroDirty) {
    if(abs(pressure-average) < 0.15) {
      baroDir=BARO_STEADY;
    } else if(pressure > average) {
      baroDir=BARO_RISING;
    } else {
      baroDir=BARO_FALLING;
    }



    uint8_t y_offset=30;
    redrawBackgroundSection(x_org+15,y_org+y_offset,270,85);

    tft->textMode();
    setArialFont();
    tft->textEnlarge(1);

    char buffer[6];

    if(displayMode == HPA_MODE) {
      sprintf(buffer,"%5.1f", pressure);
      if(pressure < 1000.0)
        tft->textSetCursor(x_org+45,y_org+y_offset);
      else
        tft->textSetCursor(x_org+15,y_org+y_offset);
    } else {
        float hgPress = pressure / HPA_HG_CONVERSION;
        sprintf(buffer,"%4.2f", hgPress);
        tft->textSetCursor(x_org+35,y_org+y_offset);

    }
    printString(buffer);

    switch(baroDir) {
      case BARO_RISING:
        drawTransparentBitmap(x_org+235,y_org+y_offset+12,43,50, up_arrow);
        break;
      case BARO_FALLING:
        drawTransparentBitmap(x_org+235,y_org+y_offset+12,43,50, down_arrow);
        break;
      case BARO_STEADY:
        drawTransparentBitmap(x_org+235,y_org+y_offset+12,43,50, steady);
        break;
    }

    baroDirty = false;
  }

  if(extremeDirty) {
    drawExtremes();
    extremeDirty = false;
  }
}

void BaroPanel::setPressure(float baro) {
  // Compare displayed numbers
  if((++averagePoll>9)||(average == 0.0)) {
    averagePoll=0;
    getAveragePressure();
    getDailyExtremes();
  }

  int rndBaro = int(baro * 10);
  int rndPressure = int(pressure *10);

  if (rndBaro == rndPressure) {
    pressure=baro;
    return;
  }

  pressure = baro;
  baroDirty = true;

  if (pressure < 0.0)
    pressure = 0.0;


  if(pressure < low) {
    extremeDirty = true;
    low = pressure;
  }

  if(pressure > high) {
    extremeDirty = true;
    high = pressure;
  }


}

void BaroPanel::drawExtremes() {
  redrawBackgroundSection(x_org+ 1, y_org+BARO_XTREME_YOFFSET, BARO_WIDTH-1, 49);

  tft->textMode();
  tft->textTransparent(RA8875_WHITE);
  
  tft->textEnlarge(0);

  tft->textSetCursor(x_org+34,y_org+BARO_XTREME_YOFFSET);
  printString("Low");
  tft->textSetCursor(x_org+BARO_WIDTH-32-32,y_org+BARO_XTREME_YOFFSET);
  printString("High");
  
  setSmallArialFont();
  tft->textEnlarge(0);
  switch(highlow) {
    case DAILY:
      tft->textSetCursor(x_org+(BARO_WIDTH/2)-29,y_org+BARO_XTREME_YOFFSET);
      printString("Daily");
      break;
    case WEEKLY:
      tft->textSetCursor(x_org+(BARO_WIDTH/2)-34,y_org+BARO_XTREME_YOFFSET);
      printString("Weekly");
      break;
    case MONTHLY:
      tft->textSetCursor(x_org+(BARO_WIDTH/2)-39,y_org+BARO_XTREME_YOFFSET);
      printString("Monthly");
      break;
    case YEARLY:
      tft->textSetCursor(x_org+(BARO_WIDTH/2)-34,y_org+BARO_XTREME_YOFFSET);
      printString("Yearly");
      break;      
    default:
      break;
  }

  char buffer[6];
  setArialFont();
  sprintf(buffer,"%5.1f",low);
  tft->textSetCursor(x_org+5,y_org+BARO_XTREME_YOFFSET+15);
  printString(buffer);

  sprintf(buffer,"%5.1f",high);
  tft->textSetCursor(x_org+BARO_WIDTH-115,y_org+BARO_XTREME_YOFFSET+15);
  printString(buffer);

}

bool BaroPanel::isClicked(uint16_t x, uint16_t y) {

  if((x>(x_org+BARO_CLICK_MIN_X))&&(y>(y_org+BARO_CLICK_MIN_Y))&&(x<(x_org+BARO_CLICK_MAX_X))&&(y<(y_org+BARO_CLICK_MAX_Y))) {

    if(y>(y_org + BARO_XTREME_YOFFSET)) {
      switch(highlow) {
        case DAILY:
          getExtendedExtremes(7);
          highlow=WEEKLY;
          break;
        case WEEKLY:
          getExtendedExtremes(30);
          highlow=MONTHLY;
          break;
        case MONTHLY:
          getExtendedExtremes(365);
          highlow=YEARLY;
          break;
        default:
          getDailyExtremes();
          highlow=DAILY;
          break;
      }
    } else {
      if(displayMode == HPA_MODE)
        displayMode = HG_MODE;
      else 
        displayMode = HPA_MODE;
      borderDirty = true;
      baroDirty = true;
    }

    extremeDirty = true;

    draw();
    return true;
  }

  return false;
}

void BaroPanel::getDailyExtremes() {

  float newHigh,newLow;

  if(influxGetHighLowPress(1,&newHigh, &newLow))
    return;       //Some error occured

  // Round Up
  newHigh += 0.05;
  newLow += 0.05;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}

void BaroPanel::getExtendedExtremes(uint16_t timeLen) {
  float newHigh, newLow;
  getDailyExtremes();

  if(influxGetHighLowPress(timeLen,&newHigh, &newLow))
    return;       //Some error occured

  
  if(high > newHigh)
    newHigh=high;

  if(low < newLow)
    newLow = low;

  high = newHigh;
  low = newLow;

  extremeDirty = true;
}

void BaroPanel::getAveragePressure(){
  float avgPress;
  if(influxGetAveragePressure(&avgPress))
    return;     // An error has occured

  average=avgPress;
}