#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_BMP085.h>

sensors_event_t event_1;

// I2C
#define dataPin  A4 // blue SDA
#define clockPin A5 // yellow SCL

#define pin_mq_1 A0
#define DHTPIN_1 A1
#define watersensor A2
#define lightsensor A3

#define DHTTYPE DHT11


struct science{
  float mq1_ppm;
  float dht1_temp;
  float dht1_humidity;
  float bmp_pressure;
  float watersen;
  float lightsen;
};
struct science data_t = {0, 0, 0, 0, 0, 0};


DHT_Unified dht_1(DHTPIN_1, DHTTYPE);
Adafruit_BMP085 bmp;

void Sensor_Task(){
  while(true){
  data_t.mq1_ppm = (float)analogRead(pin_mq_1);
  data_t.watersen = (float)analogRead(watersensor);
  data_t.lightsen = (float)analogRead(lightsensor);

  dht_1.temperature().getEvent(&event_1);
  data_t.dht1_temp = (float)event_1.temperature;
  dht_1.humidity().getEvent(&event_1);
  data_t.dht1_humidity = (float)event_1.relative_humidity;
  if(isnan(data_t.dht1_temp) || isnan(data_t.dht1_humidity) ){
    data_t.dht1_humidity = 0;
    data_t.dht1_temp = 0;
  }
  if(data_t.bmp_pressure != NULL){
    data_t.bmp_pressure = (float)bmp.readPressure();
  }
  
  // // Printing Sensors Values 
  Serial.print("MQ_1 PPM: "); Serial.println(data_t.mq1_ppm);
  Serial.print("DHT_1 Temp: "); Serial.println(data_t.dht1_temp);
  Serial.print("DHT_1 Humidity: "); Serial.println(data_t.dht1_humidity);
  Serial.print("BMP Pressure: "); Serial.println(data_t.bmp_pressure);
  Serial.print("Water Sensor: "); Serial.println(data_t.watersen);
  Serial.print("Light Intensity: "); Serial.println(data_t.lightsen);
  // delay(1000);
  delay(500);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin();
  dht_1.begin();
  if (!bmp.begin()) {
  Serial.println("BMP Error, Check Wiring!");
    data_t.bmp_pressure = NULL;
  // while (1) {} //Infinite Loop
  }

  pinMode(pin_mq_1, INPUT);
  pinMode(watersensor, INPUT);
  pinMode(lightsensor, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Nothing
  // Send Sensors Data via ESP-NOW
    Sensor_Task();
    delay(1000);
}
