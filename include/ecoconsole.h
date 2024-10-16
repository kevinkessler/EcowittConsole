/**
 *  @filename   :   ecoconsole.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console, main include file
 *  @author     :   Kevin Kessler
 *
 * Copyright (C) 2024 Kevin Kessler
 *  
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDE_ECOCONSOLE_H_
#define INCLUDE_ECOCONSOLE_H_

#define GMT_OFFSET_SECS -18000
#define DAYLIGHT_OFFSET_SECS 3600

bool writeConf(void);
void otaSetup(void);
bool initWiFi(void);
void initDisplay(void);
void displayLoop(void);
void initMQTT(void);
void mqttLoop(void);
bool setData(char *name, char *value);
void drawAll(void);
void setError(const char *errStr);
void clearError(void);
#endif /*INCLUDE_ECOCONSOLE_H_*/
