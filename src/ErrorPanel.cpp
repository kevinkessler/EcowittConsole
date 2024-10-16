/**
 *  @filename   :   ErrorPanel.cpp
 *  @brief      :   ESP32 Weather Base Station Error Display Class
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
#include "ErrorPanel.h"
#include "Adafruit_RA8875.h"
#include "display.h"

ErrorPanel::ErrorPanel(Adafruit_RA8875 *_tft) {
  tft = _tft;

  strcpy(messageBuffer,"Error: An Error has occured");
  messageExists=false;
}

void ErrorPanel::draw() {

  if(messageExists) {
    tft->fillRect(0,459,799,20,RA8875_RED);

    tft->textMode();
    tft->textTransparent(RA8875_WHITE);

    uint16_t xPos = 400 - (strlen(messageBuffer)/2 *10);
    tft->textSetCursor(xPos,459);

    printString(messageBuffer);
  } else {
    redrawBackgroundSection(0,459,799,20);
  }
}

void ErrorPanel::setMessage(const char *error) {
  Serial.print("Err Set Message ");Serial.println(error);

  strncpy(messageBuffer,error, 69);
  if(strlen(error)>69)
    messageBuffer[70] = '\0';
    
  messageExists=true;
  draw();
}

void ErrorPanel::clearMessage() {
  messageExists=false;
  draw();
}