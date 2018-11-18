#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <SD.h>

//Define DHT
#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22  
DHT_Unified dht(DHTPIN, DHTTYPE);

//Define OLED

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);



//Define WIFI
const char* ssid = "League";                   // wifi ssid
const char* password =  "ali12345";         // wifi password
const char* mqttServer = "192.168.0.107";    // IP adress Raspberry Pi
const int mqttPort = 1883;
const char* mqttUser = "mqtt1";      // if you don't have MQTT Username, no need input
const char* mqttPassword = "mqtt1";  // if you don't have MQTT Password, no need input

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t delayMS;


void setup() {
  // Serial Begin
  Serial.begin(9600); 

  // Initialize DHT
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;


  //Initialize OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  
  
  Serial.println("Connected to the WiFi network");

}

// Call Back function:
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

}

  
void loop() {
    // Delay between measurements.
    delay(delayMS);

   


   dispC("Temp......", 100);
   dispNC(temperature,2000);
   
   dispC("Humidity..", 100);
   dispNC(humidity, 2000);
   
    client.publish("sens/temp", sendTemp);
    client.publish("sens/hum", sendHum);
    delay(5000);
  client.loop();
   
  
}

void dispC(String msg,int delays){

   display.clearDisplay();
   display.setCursor(0,10);
   display.println(msg);
   display.display();
   delay(delays);
}

void dispNC(String msg, int delays){
   display.println(msg);
   display.display();
   delay(delays);
   
}

void saveData(data){
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
   }
}

void connectWifi(){
  //Initialize Wifi
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    
    dispC("Connecting", 100);
    dispNC("To Wifi",1500);
  }
}

void connectMQTT(){

    //Initialize MQTT and Connect

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    display.clearDisplay();
    display.setCursor(0,10);
    dispC("Connecting", 100);
    dispNC("To MQTT", 1500);

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");
      dispC("Connected", 500);
      

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
 }
}


void getTemp(){


  // Get temperature event and print its value. 
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  String temperature = String(event.temperature);
  String temp = "temp " + String(event.temperature);
  
  dht.humidity().getEvent(&event);
  String humidity = String(event.relative_humidity);
  String hum = "Hum " + String(event.relative_humidity);
  
  const char* sendTemp = temperature.c_str();
  const char* sendHum =  humidity.c_str();

  Serial.println(sendTemp);
  Serial.println(sendHum);

  return temperature, sendTemp, humidity, sendHum

}
