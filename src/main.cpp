/**
 *  @filename   :   main.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console, main module
 *  @author     :   Kevin Kessler
 *
 * Copyright (C) 2024 Kevin Kessler
 *
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
#include <Arduino.h>
#include <Preferences.h>
#include <ElegantOTA.h>
#include "ecoconsole.h"
#include <SPI.h>

//SET_LOOP_TASK_STACK_SIZE(16*1024);

Preferences conf;
String MQTT_SERVER="mqtt.lan";
String INFLUX_SERVER="influx.lan";
String INFLUX_TOKEN="AAAAAAA";
String MQTT_TOPIC="ABCXYZABCXYZABCXYZABCXYZABCXYZ";
String SSID = "";
String PASS = "";
uint16_t MQTT_PORT=1883;
extern WebServer webServer;


bool writeConf() {

  if (!conf.begin("config", false)) {
    setError("Preferences Failure");
    return false;
  }

  conf.putString("MQTT_SERVER",MQTT_SERVER);
  conf.putString("INFLUX_SERVER",INFLUX_SERVER);
  conf.putString("INFLUX_TOKEN",INFLUX_TOKEN);
  conf.putString("MQTT_TOPIC",MQTT_TOPIC);
  conf.putUShort("MQTT_PORT",MQTT_PORT);
  conf.putString("SSID",SSID);
  conf.putString("PASS",PASS);

  conf.end();
  return true;
}

void loadConf(){
  conf.begin("config", false);

  if(!conf.isKey("MQTT_SERVER")) {
    conf.end();
    writeConf();
  } else {
    MQTT_SERVER = conf.getString("MQTT_SERVER");
    INFLUX_SERVER = conf.getString("INFLUX_SERVER");
    INFLUX_TOKEN = conf.getString("INFLUX_TOKEN");
    MQTT_TOPIC = conf.getString("MQTT_TOPIC");
    SSID = conf.getString("SSID");
    PASS = conf.getString("PASS");
    MQTT_PORT = conf.getUShort("MQTT_PORT");
    conf.end();
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println("Begin");

  SPI.begin(9,8,10,5); // Bodge to fix schematic error (SCK and MISO switched)

  initDisplay();
  
  loadConf();

  bool inited = false; 
  do {
    inited = initWiFi();
  } while(!inited);

  otaSetup();

  initMQTT();
}


void loop() {

  if(WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }

  displayLoop();
  mqttLoop();

  webServer.handleClient();
  ElegantOTA.loop();
}

