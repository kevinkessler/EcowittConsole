/**
 *  @filename   :   WindPanel.cpp
 *  @brief      :   ESP32 Weather Base Station Wind Panel Class
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
#include "WindPanel.h"
#include "Adafruit_RA8875.h"
#include "display.h"

WindPanel::WindPanel(Adafruit_RA8875 *_tft, uint16_t _x, uint16_t _y) {
  tft = _tft;
  x_org = _x;
  y_org = _y;
  wind = 0.0;
  direction[0]='N';direction[1]='\0';

  gust = 0.0;
  maxGust = 0.0;
  displayMode=WIND_MODE;

  windDirty = true;
  borderDirty = true;
}

void WindPanel::draw() {

  if(borderDirty) {
    tft->graphicsMode();
    tft->drawLine(x_org,y_org, x_org,y_org+WIND_HEIGTH-25,RA8875_YELLOW); // Left
    tft->drawLine(x_org+WIND_WIDTH,y_org, x_org+WIND_WIDTH,y_org+WIND_HEIGTH-25,RA8875_YELLOW); // Right
    tft->drawLine(x_org+25,y_org+WIND_HEIGTH, x_org+WIND_WIDTH-25,y_org+WIND_HEIGTH,RA8875_YELLOW); //Bottom
    tft->drawCurve(x_org+25, y_org+WIND_HEIGTH-25, 25, 25,0,RA8875_YELLOW);
    tft->drawCurve(x_org+WIND_WIDTH-25, y_org+WIND_HEIGTH-25, 25, 25,3,RA8875_YELLOW);

    tft->textMode();
    tft->textTransparent(RA8875_WHITE);
    tft->textEnlarge(0);
    
    redrawBackgroundSection(x_org + 14, y_org +3, WIND_WIDTH - 28, 20);
    tft->textSetCursor(x_org+(WIND_WIDTH-40)/2, y_org+3);

    if(displayMode==WIND_MODE)
      printString("Wind");
    else if(displayMode==GUST_MODE)
      printString("Gust");
    else if(displayMode == MAXGUST_MODE) {
      tft->textSetCursor(x_org+(WIND_WIDTH-40)/2 - 20, y_org+3);
      printString("Max Gust");
    }

    borderDirty = false;
  }

  if(windDirty) {
    redrawBackgroundSection(x_org+ 25, y_org+30, WIND_WIDTH-30, WIND_HEIGTH - 42);
    //tft->drawRect(x_org+ 25, y_org+30, WIND_WIDTH-30, WIND_HEIGTH - 42, RA8875_GREEN);

    tft->textMode();
    setArialFont();
    tft->textEnlarge(1);
    tft->textTransparent(RA8875_WHITE);
    tft->textSetCursor(x_org+35,y_org+30);


    char buffer[4];
    float value;
    switch(displayMode) {
      case WIND_MODE:
        value = wind;
        break;
      case GUST_MODE:
        value = gust;
        break;
      case MAXGUST_MODE:
        value = maxGust;
        break;
      default:
        value = wind;
        break;
    }
    if(value < 10) {
      sprintf(buffer,"%2.1f",value);
    } else if(value <100) {
      sprintf(buffer,"%2.0f",value);
    } 

    printString(buffer);

    tft->textEnlarge(0);
    tft->textSetCursor(x_org+160, y_org+50);

    if(strlen(direction)==1)
      tft->textSetCursor(x_org+192, y_org+50);
    
    if(strlen(direction) == 2)
      tft->textSetCursor(x_org+176, y_org+50);
    printString(direction);

    windDirty = false;
  }
  
}

bool WindPanel::isClicked(uint16_t x, uint16_t y) {
  if((x>(x_org+WIND_CLICK_MIN_X))&&(y>(y_org+WIND_CLICK_MIN_Y))&&(x<(x_org+WIND_CLICK_MAX_X))&&(y<(y_org+WIND_CLICK_MAX_Y))) {
    switch(displayMode) {
      case WIND_MODE:
        displayMode = GUST_MODE;
        break;
      case GUST_MODE:
        displayMode = MAXGUST_MODE;
        break;
      case MAXGUST_MODE:
        displayMode = WIND_MODE;
        break;
      default:
        displayMode = WIND_MODE;
    }
    borderDirty = true;
    windDirty = true;
    draw();
    return true;
  }

  return false;
}

void WindPanel::setWind(float _wind) {

  if (wind == _wind)
    return;

  wind = _wind;
  if(wind < 0) {
    wind = 0;
  }

  if(displayMode == WIND_MODE)
    windDirty = true;

}

void WindPanel::setGust(float _gust) {

  if (gust == _gust)
    return;

  gust = _gust;
  if(_gust < 0) {
    gust = 0;
  }

  if(displayMode == GUST_MODE)
    windDirty = true;

}

void WindPanel::setMaxGust(float _maxGust) {

  if (maxGust == _maxGust)
    return;

  maxGust = _maxGust;
  if(maxGust < 0) {
    maxGust = 0;
  }

  if(displayMode == MAXGUST_MODE)
    windDirty = true;

}

void WindPanel::setDirection(char *_dir) {
  if(strcmp(_dir, direction)==0) {
    return;
  }

  strncpy(direction,_dir,5);
  windDirty = true;
}