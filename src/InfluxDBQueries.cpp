/**
 *  @filename   :   InfluxDBQueries.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console InfluxDB Queries Class
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

#include <Arduino.h>
#include <HTTPClient.h>
#include "InfluxDBQueries.h"
#include "display.h"
#include "time.h"

float h,l;

extern String INFLUX_SERVER;
extern String INFLUX_TOKEN;

const char *floatQuery="/query?db=weather&q=SELECT%%20%s%%28%%22value%%22%%29%%20from%%20%%22mqtt_consumer%%22%%20WHERE%%20time%%3E%%3Dnow%%28%%29-%d%s%%20AND%%20entity_id%%3D%%27%s%%27";

uint16_t doFloatQuery(const char *url, float *f) {
  HTTPClient hc;

  hc.begin(url);
  hc.setAuthorizationType("Token");
  hc.setAuthorization(INFLUX_TOKEN.c_str());
  hc.addHeader("Accept","application/csv");
  int rc=hc.GET();


  if(rc == 200) {

    String payload=hc.getString();
    char buffer[strlen(payload.c_str())];
    strncpy(buffer,payload.c_str(),strlen(payload.c_str()));
    char * prev;
    char *tok=strtok(buffer,",");
    while(tok!=NULL) {
      prev=tok;
      tok=strtok(NULL,",");
    }

    for(int n=0;n<strlen(prev);n++)     //Replace trailing \n with 0
        if(prev[n]==10)
            prev[n]=0;

    char *ending;
    if(prev!=NULL){
      *f=strtof(prev,&ending);
      if(*ending!=0) {
        Serial.print("Ending bad ");Serial.println(*ending);
        rc = 500;
      }
    }
    else
      rc = 500;


  }

  hc.end();

  return rc;

}

uint16_t influxGetFloatMaxMin(const char *column, uint16_t days, float *high, float *low) {
  char url[256];
  char uri[256];


  sprintf(uri,floatQuery,"MAX",days,"d",column);
  sprintf(url,"http://%s:8086%s",INFLUX_SERVER,uri);
  uint16_t n=doFloatQuery(url, high);

  if(n!=200)
  {
    char error[50];
    sprintf(error,"Getting MAX %s returned %d",column,n);
    setError(error);

    return n;
  }
  sprintf(uri,floatQuery,"MIN",days,"d",column);
  sprintf(url,"http://%s:8086%s",INFLUX_SERVER,uri);
  n=doFloatQuery(url, low);

  if(n!=200)
  {
    char error[50];
    sprintf(error,"Getting MIN %s returned %d",column,n);
    setError(error);
  }
  return n;

}

uint8_t influxGetAveragePressure(float *ave) {
  char url[256],uri[256];

  sprintf(uri,floatQuery,"MEAN",2,"h","baromrel");
  sprintf(url,"http://%s:8086%s",INFLUX_SERVER,uri);

  uint8_t retval = doFloatQuery(url,ave);

  return retval;
}

uint8_t influxGetHighLowTemp(bool indoor, uint16_t days, float *high, float *low) {
  uint8_t retval = 0;
  const char *col;

  if(indoor)
    col = "tempin";
  else
    col = "temp";
  retval = influxGetFloatMaxMin(col, days, high, low);


  return (retval!=200);
} 

uint8_t influxGetHighLowPress(uint16_t days, float *high, float *low) {
  uint8_t retval = 0;

  retval = influxGetFloatMaxMin("baromrel", days, high, low);


  return (retval!=200);
}

uint8_t influxGetHighLowHum(bool indoor, uint16_t days, float *high, float *low) {
  uint8_t retval = 0;
  const char *col;

  if(indoor)
    col = "humidityin";
  else
    col = "humidity";
  retval = influxGetFloatMaxMin(col, days, high, low);


  return (retval!=200);
} 