#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>


#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
unsigned long currentMillis;

WiFiClient espClient;
PubSubClient client(espClient);
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}


unsigned long old_mils = 60000;

void reconnect() 
{
  // Loop until we're reconnected
    if (!client.connected())
    { 
        if (millis() - old_mils > 60000)
        {
            old_mils = millis();
            Serial.print("Attempting MQTT connection...");
            // Create a random client ID
            String clientId = "ESP_hormann_garage_door";
            
            clientId += String(random(0xffff), HEX);
            // Attempt to connect
            if (client.connect(clientId.c_str(),"mqttuser", "mqttuser")) 
            {
                Serial.println("connected to MQTT server");
                // MQTT subscription
                
                client.subscribe("avshrs/sensors/hormann_garage_door_01/set/light");
                client.subscribe("avshrs/sensors/hormann_garage_door_01/esp_led");
                client.subscribe("avshrs/sensors/hormann_garage_door_01/set/door");

            } 
        }
    }
}

String uptime(unsigned long milli)
{
  static char _return[32];
  unsigned long secs=milli/1000, mins=secs/60;
  unsigned int hours=mins/60, days=hours/24;
  milli-=secs*1000;
  secs-=mins*60;
  mins-=hours*60;
  hours-=days*24;
  sprintf(_return,"Uptime %d days %2.2d:%2.2d:%2.2d.%3.3d", (byte)days, (byte)hours, (byte)mins, (byte)secs, (int)milli);
  String ret =  _return;
  return ret;
}


void wifi_status()
{
    String ip = IpAddress2String(WiFi.localIP());
    client.publish("avshrs/sensors/hormann_garage_door_01/status/wifi_ip", ip.c_str());
    String mac = WiFi.macAddress();
    client.publish("avshrs/sensors/hormann_garage_door_01/status/wifi_mac", mac.c_str());
    snprintf (msg, MSG_BUFFER_SIZE, "%i", WiFi.RSSI());
    client.publish("avshrs/sensors/hormann_garage_door_01/status/wifi_rssi", msg);
    int signal = 2*(WiFi.RSSI()+100);
    snprintf (msg, MSG_BUFFER_SIZE, "%i", signal);
    client.publish("avshrs/sensors/hormann_garage_door_01/status/wifi_signal_strength", msg);
    
    client.publish("avshrs/sensors/hormann_garage_door_01/status/uptime", uptime(currentMillis).c_str());
}


void prepare_conf()
{
    char s1[] = "{\"avty\":{\"topic\":\"avshrs/sensors/hormann_garage_door_01/status/connected\",\"payload_available\":\"true\",\"payload_not_available\":\"false\"},\"~\":\"avshrs/sensors/hormann_garage_door_01\",\"device\":{\"ids\":\"garage_door_sensor-01\",\"mf\":\"Avshrs\",\"name\":\"SupraMatic3-01\",\"sw\":\"0.0.1\"},\"name\":\"Garage door\",\"uniq_id\":\"hormann_garage_door_01\",\"qos\":0,\"command_topic\":\"~/set/door\",\"position_topic\":\"~state/door\",\"device_class\":\"cover\",\"payload_open\":\"OPEN\",\"payload_close\":\"CLOSE\",\"payload_stop\":\"STOP\",\"position_open\":\"100\",\"position_closed\":\"0\",\"position_template\": \">-{% if value == \"open\" %}100{% elif value == \"closed\" %}0{% else %}10{% endif %}\"}";
    char s2[] = "{\"avty\":{\"topic\":\"avshrs/sensors/hormann_garage_door_01/status/connected\",\"payload_available\":\"true\",\"payload_not_available\":\"false\"},\"~\":\"avshrs/sensors/hormann_garage_door_01\",\"device\":{\"ids\":\"garage_door_sensor-01\",\"mf\":\"Avshrs\",\"name\":\"SupraMatic3-01\",\"sw\":\"0.0.1\"},\"name\":\"Garage door\",\"uniq_id\":\"hormann_garage_door_01\",\"qos\":0,\"command_topic\":\"~/set/venting\",\"state_topic\":\"~state/venting\",\"device_class\":\"cover\",\"payload_open\":\"OPEN\",\"payload_close\":\"CLOSE\",\"payload_stop\":\"STOP\",\"position_open\":\"100\",\"position_closed\":\"0\",\"position_template\": \">-{% if value == \"open\" %}100{% elif value == \"closed\" %}0{% else %}10{% endif %}\"}";
    

}

void setup_wifi() 
{
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
