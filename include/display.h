/**
 *  @filename   :   display.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, display module
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
#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include "PanelBase.h"

#define CS 5
#define RST 21
#define WAIT_PIN 20

extern const uint8_t background_bmp[];
extern const uint8_t therm_bmp[];
extern const uint8_t hg_bmp[];
extern const uint8_t battery[];
extern const uint8_t rain[];
extern const uint8_t up_arrow[];
extern const uint8_t down_arrow[];
extern const uint8_t steady[];

struct PanelList {
  PanelBase *p;
  PanelList *next;
};

void redrawBackgroundSection(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void printString(const char *s);
void setArialFont(void);
void setSmallArialFont(void);
void drawTransparentBitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *bitmap);
void drawCenteredArial(uint16_t centerx, uint16_t centery, int8_t value);
void setError(const char *errStr);
void tftCTPTouch(uint16_t x, uint16_t y);

#endif /* INCLUDE_DISPLAY_H_ */