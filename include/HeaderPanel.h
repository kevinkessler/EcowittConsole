/**
 *  @filename   :   HeaderPanel.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, Header Panel Class
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
#ifndef INCLUDE_HEADERPANEL_H_
#define INCLUDE_HEADERPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"
#include "PanelBase.h"

#define HEADER_HEIGTH 10

class HeaderPanel: virtual public PanelBase {
  public:
    HeaderPanel(Adafruit_RA8875 *tft);
    void draw(void);
    void setBatteryLevel(float level);
    bool isClicked(uint16_t x, uint16_t y) override;
    
  private:
    Adafruit_RA8875 *tft;
    char timeBuffer[6];
    char dateBuffer[9];
    float battery_level;
    void fillDateTimeBuffers(void);
};

#endif /* INCLUDE_HEADERPANEL_H_ */