#include <string.h>
#include <Wire.h>
#include "passwd.h"
#include "wifi_mqtt.h"
#include "hoermann.h"


int d = 1; 
unsigned long previousMillis = 0;  
unsigned long previousMillis2 = 0;  
Hoermann hoermann; 
String state = "n/a";


void callback(char* topic, byte* payload, unsigned int length) 
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String st;
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        st +=(char)payload[i];
    }

    Serial.println();
    if (strcmp(topic,"avshrs/sensors/hormann_garage_door_01/set/door")) 
    {   
        hoermann.set_state(st);

    } 
    else if (strcmp(topic,"avshrs/sensors/hormann_garage_door_01/Esp_led") == 0 && (char)payload[0] == '1') 
    {
        digitalWrite(BUILTIN_LED, LOW);   
    } 
    else if (strcmp(topic,"avshrs/sensors/hormann_garage_door_01/Esp_led") == 0 && (char)payload[0] == '0') 
    {
        digitalWrite(BUILTIN_LED, HIGH); 
    }


}

void setup() 
{
    int EnTxPin =  18;
    Wire.begin();
    hoermann.init(EnTxPin);

    Serial.begin(576000);
    
    // Serial2.begin(19200);
    // Serial2.setTimeout(2);
    // Serial2.setRxBufferSize(10);
    
    
    pinMode(EnTxPin, OUTPUT);  
    digitalWrite(EnTxPin, LOW);
    pinMode(BUILTIN_LED, OUTPUT);  
    digitalWrite(BUILTIN_LED, LOW);   
    setup_wifi();

    client.setServer(mqtt_server, 1883);
    client.setBufferSize(1024);
    client.setCallback(callback);
}




void loop() 
{
    currentMillis = millis();

    if (!client.connected()) 
    {
        reconnect();
    }
    client.loop();
    hoermann.run_loop();
    
    
    
    if (currentMillis - previousMillis >= 60000) 
    {
        previousMillis = currentMillis;

        wifi_status();

        snprintf (msg, MSG_BUFFER_SIZE, "true");
        client.publish("avshrs/sensors/hormann_garage_door_01/status/connected", msg);
        client.publish("avshrs/sensors/hormann_garage_door_01/state/door", hoermann.get_state().c_str());
    }
    
    if (state != hoermann.get_state()) 
    {
        state = hoermann.get_state();
        client.publish("avshrs/sensors/hormann_garage_door_01/state/door", hoermann.get_state().c_str());
    }
    

    
   
}
