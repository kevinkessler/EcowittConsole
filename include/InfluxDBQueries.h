/**
 *  @filename   :   InfluxDBQueries.h
 *  @brief      :   ESP32 Ecowitt Weather Station Console,  InfluxDB Queries Class
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

#ifndef INCLUDE_INFLUXDBQUERIES_H_
#define INCLUDE_INFLUXDBQUERIEs_H_

uint8_t influxGetHighLowTemp(bool indoor, uint16_t days, float *high, float *low);
uint8_t influxGetAveragePressure(float *ave);
uint8_t influxGetHighLowPress(uint16_t days, float *high, float *low);
uint8_t influxGetHighLowHum(bool indoor, uint16_t days, float *high, float *low);


#endif /* INCLUDE_INFLUXDBQUERIEs_H_ */