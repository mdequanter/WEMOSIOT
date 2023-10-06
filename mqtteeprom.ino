#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // I2C
#include <ArduinoJson.h>


WiFiClient espClient;
PubSubClient client(espClient);


const int SSID_SIZE = 32;
const int PASS_SIZE = 64;  
const int MQTT_SIZE = 32;
const int TOPIC_SIZE = 32;
const int LOC_SIZE = 32;
const int DEVICE_SIZE = 32;



char ssid[SSID_SIZE];
char pass[PASS_SIZE];
char mqtt_server[MQTT_SIZE];
char mqtt_topic[TOPIC_SIZE];
char location[LOC_SIZE];
char device_name[DEVICE_SIZE];

void setup() {

  Serial.begin(115200);
  unsigned status;
  status = bmp.begin(0x76);



  
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(SSID_SIZE, pass);
  EEPROM.get(SSID_SIZE+PASS_SIZE, mqtt_server);
  EEPROM.get(SSID_SIZE+PASS_SIZE+MQTT_SIZE, mqtt_topic);
  EEPROM.get(SSID_SIZE+PASS_SIZE+MQTT_SIZE+TOPIC_SIZE, location);
  EEPROM.get(SSID_SIZE+PASS_SIZE+MQTT_SIZE+TOPIC_SIZE+LOC_SIZE, device_name);

  WiFi.begin(ssid, pass);




  Serial.println("");
  Serial.println("WiFi connected");

  Serial.println("Device Started...");

  client.setServer(mqtt_server, 1883);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter++;
    Serial.println(counter);
    if (counter > 20) {
      setupWifi();
    }
  }


  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), "", "")) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }

}


void loop() {


  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    
    if (cmd == "setup") {
      setupWifi();
    }
    
    else if (cmd == "readsetup") {
      readSettings();
    }
  }

  String temperature;
  String pressure;
  String sensordata;

  float t = bmp.readTemperature();
  float p = bmp.readPressure();
  temperature = String(t,2);
  if (p > 0){
    p=p/100;
  } else {
    p=0;
  }
  pressure = String(p,2);

  float battery = analogRead(A0);   // read the input on analog pin 0
  if (battery > 1000){
    battery = 1000;
  }

// Create JSON object
  StaticJsonDocument<200> doc;

  doc["device_name"] = device_name;
  doc["battery_voltage"] = battery;
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;

  // Serialize JSON to string
  char json[256];
  serializeJson(doc, json);

  // Publish JSON

  Serial.print("MQTT Server: ");
  Serial.println(mqtt_server);

  Serial.print("MQTT Topic: ");
  Serial.println(mqtt_topic);

  Serial.print("sending:");
  Serial.println(json);

  if(client.publish(mqtt_topic, json)){
  Serial.println("Publish ok"); 


}
else {
  Serial.println("Publish failed");
}

  delay(1000);

  Serial.println("going to deepsleep");
  ESP.deepSleep(1800e6); 


}

void setupWifi() {

  Serial.println("Enter SSID:");
  readSerial(ssid, SSID_SIZE);
  
  Serial.println("Enter Password:");
  readSerial(pass, PASS_SIZE); 

  Serial.println("Enter MQTT Server:");
  readSerial(mqtt_server, MQTT_SIZE);
  
  Serial.println("Enter MQTT Topic:");
  readSerial(mqtt_topic, TOPIC_SIZE);

  Serial.println("Enter Location:");
  readSerial(location, LOC_SIZE);
   
  Serial.println("Enter Device Name:");
  readSerial(device_name, DEVICE_SIZE);

  EEPROM.put(0, ssid);
  EEPROM.put(SSID_SIZE, pass);
  EEPROM.put(SSID_SIZE+PASS_SIZE, mqtt_server); 
  EEPROM.put(SSID_SIZE+PASS_SIZE+MQTT_SIZE, mqtt_topic);
  EEPROM.put(SSID_SIZE+PASS_SIZE+MQTT_SIZE+TOPIC_SIZE, location);
  EEPROM.put(SSID_SIZE+PASS_SIZE+MQTT_SIZE+TOPIC_SIZE+LOC_SIZE, device_name);
  
  EEPROM.commit();

  Serial.println("Settings saved!");

}

void readSerial(char* buffer, int maxLen) {
  Serial.setTimeout(50000);
  int count=0;
  while (count < maxLen-1) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c=='\n') break;
      buffer[count++] = c; 
    }
  }
  buffer[count] = '\0';
}

void readSettings() {

  Serial.print("SSID: ");
  Serial.println(ssid);

  Serial.print("Password: ");
  Serial.println("xxxxxxxx");  

  Serial.print("MQTT Server: ");
  Serial.println(mqtt_server);

  Serial.print("MQTT Topic: ");
  Serial.println(mqtt_topic);

  Serial.print("Location: ");
  Serial.println(location);

  Serial.print("Device Name: ");
  Serial.println(device_name);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

}
