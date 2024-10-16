/**
 *  @filename   :   wifi.cpp
 *  @brief      :   ESP32 Ecowitt Weather Station Console, wifi module
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


#include <ElegantOTA.h>
#include "ecoconsole.h"

WebServer webServer;

const char *hostname="EcoConsole";
extern String MQTT_SERVER;
extern String INFLUX_SERVER;
extern String INFLUX_TOKEN;
extern String MQTT_TOPIC;
extern String SSID;
extern String PASS;
extern uint16_t MQTT_PORT;

bool initWiFi() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.setHostname(hostname);

  if((SSID != "")&&(PASS!="")) {
    WiFi.begin(SSID.c_str(), PASS.c_str());
    for (int n=0; n<20; n++) {
      Serial.print(".");
      if(WiFi.status()==WL_CONNECTED)
        break;
      delay(500);
    }    
  }

  if(WiFi.status()!=WL_CONNECTED) {
    WiFi.beginSmartConfig();
    setError("Waiting for Smart Config");
    for(int n=600; n && (!WiFi.smartConfigDone()); --n) {
      Serial.print("x");
      delay(500);
    }

    for (int n=0; n<10; n++) {
      Serial.print(".");
      if(WiFi.status()==WL_CONNECTED)
        break;
      delay(500);
    }

    if(WiFi.status()==WL_CONNECTED) {
      SSID = WiFi.SSID();
      PASS = WiFi.psk();
      writeConf();
    } 
  }

  if(WiFi.status()==WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    Serial.println(WiFi.localIP());
    clearError();
    return true;
  } else

  setError("Failure Connecting WiFi");
  return false;
}



void otaSetup() {
  webServer.on("/", [](){
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<a href=\"/update\">Upload Firmware</a></center>";
    message += "<center><a href=\"/resetwifi\">Reset Wifi Settings</a></center>";
    message += "<center><a href=\"/restart\">Restart ESP32</a></center>";

    message += "<form action=\"/mqttserver\"><center>MQTT Server <input type=\"text\" name=\"mqttserver\" value=\"";
    message += MQTT_SERVER;
    message += "\"><input type=\"submit\" value=\"Submit\"></form>";

    message += "<form action=\"/mqtttopic\"><center>MQTT Topic <input type=\"text\" name=\"mqtttopic\" value=\"";
    message += MQTT_TOPIC;
    message += "\"><input type=\"submit\" value=\"Submit\"></form>";

    message += "<form action=\"/mqttport\"><center>MQTT Port <input type=\"number\" name=\"mqttport\" value=\"";
    message += MQTT_PORT;
    message += "\"><input type=\"submit\" value=\"Submit\"></form>";

    message += "<form action=\"/influxServer\"><center>InfluxDB Server <input type=\"text\" name=\"influxServer\" value=\"";
    message += INFLUX_SERVER;
    message += "\"><input type=\"submit\" value=\"Submit\"></form>";

    message += "<form action=\"/influxToken\"><center>InfluxDB Token <input type=\"text\" name=\"influxToken\" value=\"";
    message += INFLUX_TOKEN;
    message += "\"><input type=\"submit\" value=\"Submit\"></form>";
    message +="</body></html>";

    webServer.send(200,"text/html",message);
  });

  webServer.on("/resetwifi", []() {
    SSID = "";
    PASS = "";
    writeConf();
    webServer.send(200,"text/plain","OK");
    ESP.restart();
  });

  webServer.on("/restart", []() {
    webServer.send(200,"text/plain","OK");
    ESP.restart();
  });

  webServer.on("/mqttserver", []() {
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<br><center></h2> Save of MQTT Server ";
    message += webServer.arg(0);
    message += " successful </h2></center></body></html>";

    bool retval=true;
    if(webServer.argName(0)=="mqttserver") {
        MQTT_SERVER=webServer.arg(0);
        retval=writeConf();
    } else {
        retval = false;
    }
    if(retval)
        webServer.send(200,"text/html",message);
    else
        webServer.send(500,"text/plain","Error occured saving Preferences or parameter name incorrect");
  });


  webServer.on("/mqtttopic", []() {
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<br><center></h2> Save of MQTT Topic ";
    message += webServer.arg(0);
    message += " successful </h2></center></body></html>";

    bool retval=true;
    if(webServer.argName(0)=="mqtttopic") {
        MQTT_TOPIC=webServer.arg(0);
        retval=writeConf();
    } else {
        retval = false;
    }
    if(retval)
        webServer.send(200,"text/html",message);
    else
        webServer.send(500,"text/plain","Error occured saving EEPROM or parameter name incorrect");
  });

  webServer.on("/mqttport", []() {
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<br><center></h2> Save of MQTT Port ";
    message += webServer.arg(0);
    message += " successful </h2></center></body></html>";

    bool retval=true;
    uint16_t port=strtoul(webServer.arg(0).c_str(),NULL,10);
  
    if((webServer.argName(0) == "mqttport") && (port !=0)) {
        MQTT_PORT = port;
        retval=writeConf();
    } else {
        retval = false;
    }
    if(retval)
        webServer.send(200,"text/html",message);
    else
        webServer.send(500,"text/plain","Error occured saving EEPROM or parameter name incorrect");
  });

  webServer.on("/influxServer", []() {
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<br><center></h2> Save of InfluxDB Server ";
    message += webServer.arg(0);
    message += " successful </h2></center></body></html>";

    bool retval=true;
    if(webServer.argName(0)=="influxServer") {
        INFLUX_SERVER=webServer.arg(0);
        retval = writeConf();
    } else {
        retval = false;
    }
    if(retval)
        webServer.send(200,"text/html",message);
    else
        webServer.send(500,"text/plain","Error occured saving EEPROM or parameter name incorrect");
  });

  webServer.on("/influxToken", []() {
    String message="<!DOCTYPE html><html>\n<head><title>";
    message += hostname;
    message += "</title></head>\n<body><center><H1>";
    message += hostname;
    message += " Configuration</H1></center><br>\n<center>";
    message += "<br><center></h2> Save of InfluxDB Token ";
    message += webServer.arg(0);
    message += " successful </h2></center></body></html>";

    bool retval=true;
    if(webServer.argName(0) == "influxToken") {
        INFLUX_TOKEN = webServer.arg(0);
        retval = writeConf();
    } else {
        retval = false;
    }
    if(retval)
        webServer.send(200,"text/html",message);
    else
        webServer.send(500,"text/plain","Error occured saving EEPROM or parameter name incorrect");
  });

  ElegantOTA.begin(&webServer);
  webServer.begin();
} 