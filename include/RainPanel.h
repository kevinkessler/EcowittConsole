/**
 *  @filename   :   RainPanel.h
 *  @brief      :   ESP32 Weather Base Station Rain Gauge Panel Class
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
#ifndef INCLUDE_RAINPANEL_H_
#define INCLUDE_RAINPANEL_H_

#include <Arduino.h>
#include "Adafruit_RA8875.h"
#include "PanelBase.h"

#define RAIN_WIDTH 289
#define RAIN_HEIGTH 130
#define RAIN_CLICK_MIN_X 1
#define RAIN_CLICK_MIN_Y (40)
#define RAIN_CLICK_MAX_X (RAIN_WIDTH-1)
#define RAIN_CLICK_MAX_Y (RAIN_HEIGTH - 1) 


class RainPanel: virtual public PanelBase {
  public:
    RainPanel(Adafruit_RA8875 *tft, uint16_t x, uint16_t y);
    void draw(void);
    void setDailyRain(float rain);
    void setWeeklyRain(float rain);
    void setMonthlyRain(float rain);
    void setYearlyRain(float rain);
    bool isClicked(uint16_t x, uint16_t y) override;

  private:
    Adafruit_RA8875 *tft;
    uint16_t x_org;
    uint16_t y_org;
    float drain;
    float wrain;
    float mrain;
    float yrain;
    uint8_t refreshCount;

    enum Extremes rainPeriod;
    bool rainDirty;
    bool borderDirty;

    void getDailyRain(void);
    void getExtendedRain(uint16_t timeLen);
};

#endif /* INCLUDE_RAINPANEL_H_ */
