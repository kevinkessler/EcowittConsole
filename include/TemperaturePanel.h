/**
 *  @filename   :   TemperaturePanel.h
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
#ifndef INCLUDE_TEMPERATUREPANEL_H_
#define INCLUDE_TEMPERATUREPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"
#include "PanelBase.h"

#define TEMP_WIDTH 250
#define TEMP_HEIGTH 230
#define TEMP_XTREME_YOFFSET 155

#define TEMP_CLICK_MIN_X 1
#define TEMP_CLICK_MIN_Y (1)
#define TEMP_CLICK_MAX_X (TEMP_WIDTH-1)
#define TEMP_CLICK_MAX_Y (50) 

#define TEMP_MODE 0
#define FEELS_MODE 1


class TemperaturePanel: virtual public PanelBase {
  public:
    TemperaturePanel(Adafruit_RA8875 *tft, uint16_t x, uint16_t y, float current, bool indoor);
    void draw(void);
    void setTemperature(float _temperature);
    void setFeelsLike(float _feels_like);
    bool isClicked(uint16_t x, uint16_t y) override;

  private:
    Adafruit_RA8875 *tft;
    uint16_t x_org;
    uint16_t y_org;
    float temperature;
    float feels_like;
    float low;
    float high;
    float lowFeels;
    float highFeels;
    bool indoor;
    uint8_t refreshCount;
    enum Extremes highlow;
    uint8_t displayMode;

    bool tempDirty;
    bool extremeDirty;
    bool borderDirty;
    bool hasData;

    void drawThermometer(uint16_t x0, uint16_t y0);
    void drawExtremes(void);
    void getDailyExtremes(void);
    void getExtendedExtremes(uint16_t timeLen);
};
#endif /* INCLUDE_TEMPERATUREPANEL_H_ */