/**
 *  @filename   :   BaroPanel.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, Barometer Panel Class
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
#ifndef INCLUDE_BAROPANEL_H_
#define INCLUDE_BAROPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"
#include "PanelBase.h"

#define BARO_WIDTH 289
#define BARO_HEIGTH 170
#define BARO_XTREME_YOFFSET 115
#define BARO_CLICK_MIN_X 1
#define BARO_CLICK_MIN_Y (1)
#define BARO_CLICK_MAX_X (BARO_WIDTH-1)
#define BARO_CLICK_MAX_Y (BARO_HEIGTH - 1) 

#define BARO_STEADY 0
#define BARO_RISING 1
#define BARO_FALLING 2

#define HPA_MODE 0
#define HG_MODE 1

#define HPA_HG_CONVERSION (33.86389)

class BaroPanel: virtual public PanelBase {
  public:
    BaroPanel(Adafruit_RA8875 *tft, uint16_t x, uint16_t y);
    void draw(void);
    void setPressure(float baro);
    bool isClicked(uint16_t x, uint16_t y) override;

  private:
    Adafruit_RA8875 *tft;
    uint16_t x_org;
    uint16_t y_org;
    float pressure;
    uint8_t baroDir;
    float low;
    float high;
    float average;
    uint8_t averagePoll;
    uint8_t displayMode;

    enum Extremes highlow;
    bool baroDirty;
    bool borderDirty;
    bool extremeDirty;

    void drawExtremes(void);
    void getDailyExtremes(void);
    void getExtendedExtremes(uint16_t timeLen);
    void getAveragePressure(void);
};

#endif /* INCLUDE_BAROPANEL_H_ */