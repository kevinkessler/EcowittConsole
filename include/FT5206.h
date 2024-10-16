/**
 *  @filename   :   FT5206.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, Touchscreen module
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

#ifndef INCLUDE_FT5206_H_
#define INCLUDE_FT5206_H_

#define FT5206_ADDR 0x38
#define FT5206_WAKE_PIN 4
#define FT5206_INT_PIN 3
#define FT5206_RST_PIN 2

#define CTP_NORMAL 0x00
#define CTP_TEST 0x04
#define CTP_SYSTEM 0x01

void ft5206_init(void);
void checkTouch(void);
ICACHE_RAM_ATTR void ft5206_interrupt(void);

#endif /* INCLUDE_FT5206_H_ */