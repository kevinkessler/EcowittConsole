/**
 *  @filename   :   mqtt.cpp
 *  @brief      :   Esp32 Ecowitt Weather Station Console, mqtt module
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

#include <WiFi.h> 
#include "PubSubClient.h"
#include "Ticker.h"
#include "ecoconsole.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);
char subName[25];

void dataDoneCallback(void);
Ticker dataDone(dataDoneCallback,2000); 

extern String MQTT_SERVER;
extern String MQTT_TOPIC;
extern uint16_t MQTT_PORT;

void dataDoneCallback() {
    dataDone.stop();
    drawAll();
}

void mqttCallback(char *topic, byte *payload, uint16_t length) {
    dataDone.stop();
    dataDone.start();

    char *token=token=strtok(topic,"/");
    char *prevToken=NULL;
    while(token!=NULL) {

        if(strcmp("state",token)==0) {

            char value[length];
            for (int n=0;n<length;n++) {
                value[n]=payload[n];
            }
            value[length] = '\0';
            setData(prevToken, value);
            break;
        }
        prevToken=token;
        token=strtok(NULL,"/");
    }


}

static void reconnect() {

    if(mqttClient.connect(subName)) {
        mqttClient.subscribe(MQTT_TOPIC.c_str());
        clearError();
    } else {
        char errorMes[50];
        sprintf(errorMes, "MQTT Connection failed, rc=%d",mqttClient.state());
        setError(errorMes);
    }
}

void disconnectMQTT() {
    mqttClient.disconnect();
}


void initMQTT() {

    mqttClient.setServer(MQTT_SERVER.c_str(), MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    sprintf(subName, "ecoconsole-%s", &(WiFi.macAddress().c_str())[9]);

}

void mqttLoop(void) {
    if(!mqttClient.connected())
        reconnect();
    
    dataDone.update();
    mqttClient.loop();
}