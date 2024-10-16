/**
 *  @filename   :   PanelBase.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console,  Panel Base Class
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
#ifndef INCLUDE_PANELBASE_H_
#define INCLUDE_PANELBASE_H_

#include <Arduino.h>

enum Extremes {DAILY, WEEKLY, MONTHLY, YEARLY};

class PanelBase {
  public:
   virtual bool isClicked(uint16_t x, uint16_t y) = 0;
   virtual void draw(void) = 0;
};

#endif /* INCLUDE_PANELBASE_H_ */