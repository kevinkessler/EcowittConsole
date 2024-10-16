/**
 *  @filename   :   ErrorPanel.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, Error Panel Class
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
#ifndef INCLUDE_ERRORPANEL_H_
#define INCLUDE_ERRORPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"

#define HEADER_HEIGTH 10

class ErrorPanel {
  public:
    ErrorPanel(Adafruit_RA8875 *tft);
    void draw(void);
    void setMessage(const char *error) ;
    void clearMessage(void);

  private:
    Adafruit_RA8875 *tft;
    char messageBuffer[70];
    bool messageExists;
};

#endif /* INCLUDE_ERRORPANEL_H_ */