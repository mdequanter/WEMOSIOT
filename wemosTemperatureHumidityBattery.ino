/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"


//#include "DHT.h"

#define DHTPIN 5    // what digital pin the DHT22 is conected to D1
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

//DHT dht(DHTPIN, DHTTYPE);

EspMQTTClient client(
  "SSID",
  "password",
  "ip",  // MQTT Broker server ip
  "mqtt_user",   // Can be omitted if not needed
  "xxxxxxxx",   // Can be omitted if not needed
  "iotbigdataDemo1",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

String temperature;
String humidity;
String battery;
String pressure;
String sensordata;

void setup()
{
  Serial.begin(115200);
  unsigned status;

  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  //client.enableLastWillMessage("relais1", "I am going offline");  // You can activate the retain flag by setting the third parameter to true

  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }
  delay(2000);



  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running sensor!");
  Serial.println("-------------------------------------");
  
}

void blinkLed(int number = 1) {
  for (int i = 0; i < number; i++) {
    digitalWrite(LED_BUILTIN, HIGH); // sets the LED on
    delay(100);                // waits for a second
    digitalWrite(LED_BUILTIN, LOW);  // sets the LED off
    delay(100);                // waits for a second
  }
}


// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient

void onConnectionEstablished()
{
    client.subscribe("temperature1", [](const String & topic, const String & payload) {
    });
    client.subscribe("pressure1", [](const String & topic, const String & payload) {
    });
    
  client.publish("online", "iotbigdata9 wake up");


  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  //float t = dht.readTemperature();
  float t = bmp.readTemperature();
  float p = bmp.readPressure();

  delay(2000);

  Serial.print("Pressure: ");
  Serial.print(p);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  temperature = String(t,2);

  if (p > 0){
    p=p/100;
  } else {
    p=0;
  }

  
  pressure = String(p,2);
  float sensorValue = analogRead(A0);   // read the input on analog pin 0
  Serial.println(sensorValue);
  if (sensorValue > 1000){
    sensorValue = 1000;
  }
  
  if (sensorValue > 0){
    sensorValue=sensorValue/10;
  } else {
    sensorValue=0;
  }
  battery = String(sensorValue);

  client.publish("online", "iotbigdata20 to deep sleep");
  
  sensordata = "{\"name\":\"iotbigdata20\",\"battery\":\""+battery+"\",\"temperature\":\""+temperature+"\",\"pressure\":\""+pressure+"\"}";

  client.publish("sensordata",sensordata);


  delay(1000);


  ESP.deepSleep(300e6); 

  delay(100);
  
}

void loop()
{
  client.loop();
}
