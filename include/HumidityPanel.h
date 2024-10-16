/**
 *  @filename   :   HumidityPanel.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, Humidity Panel Class
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
#ifndef INCLUDE_HUMIDITYPANEL_H_
#define INCLUDE_HUMIDITYPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"
#include "PanelBase.h"

#define HUM_WIDTH 250
#define HUM_HEIGTH 190
#define HUM_XTREME_YOFFSET 115
#define HUM_CLICK_MIN_X 1
#define HUM_CLICK_MIN_Y (1)
#define HUM_CLICK_MAX_X (HUM_WIDTH-1)
#define HUM_CLICK_MAX_Y (50) 

#define DP_MODE 0
#define HUM_MODE 1

class HumidityPanel: virtual public PanelBase {
  public:
    HumidityPanel(Adafruit_RA8875 *tft, uint16_t x, uint16_t y, int8_t current, bool indoor);
    void draw(void);
    void setHumidity(uint8_t humidity);
    void setDewPoint(uint8_t _dewPoint);
    bool isClicked(uint16_t x, uint16_t y) override;

  private:
    Adafruit_RA8875 *tft;
    uint16_t x_org;
    uint16_t y_org;
    int8_t humidity;
    int8_t dewPoint;
    int8_t low;
    int8_t high;
    int8_t lowDew;
    int8_t highDew;
    bool indoor;
    enum Extremes highlow;
    uint8_t displayMode;

    bool humDirty;
    bool extremeDirty;
    bool extremeDewDirty;
    bool borderDirty;
    bool hasData;

    void drawExtremes(void);
    void getDailyExtremes(void);
    void getExtendedExtremes(uint16_t timeLen);
};
#endif /* INCLUDE_HUMIDITYPANEL_H_ */