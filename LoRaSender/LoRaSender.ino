#include <SPI.h>
#include <LoRa.h> 
#include <string.h>
// #include <PubSubClient.h>

// Data Variables           // Reference Number
float mq09 = 0;             // 1
float mq135 = 0;            // 2 
float dht11 = 0;            // 3
float bmp_pressure = 0;     // 4
float bmp_altitude = 0;     // 5
float lightintensity = 0;   // 6
float pm = 0;              // 7
float water = 0;            // 8

void read_sensors(){
  // Devangs Remaining Code 
  mq09 = 80.2;
  mq135 = analogRead(A5);
  dht11 = 60.52;
  bmp_pressure = 50.05;
  bmp_altitude = 40.49;
  lightintensity = 30.00;
  pm = 20.11;
  water = 10.123;
  delay(50);
}

void LoRa_send(const char *sensorName, float value){
  char result[50];
  char valueStr[20]; 
  
  // Convert float to string with 2 decimal place 
  dtostrf(value, 6, 2, valueStr);
  // Serial.print("Value: "); 
  // Serial.println(valueStr); 
  
  strcpy(result, sensorName);
  strcat(result, ": ");
  strcat(result, valueStr);
  
  Serial.print("Packed Data: "); 
  Serial.println(result); 
  LoRa.beginPacket();  
  LoRa.print(result);
  LoRa.endPacket();

}

void setup() {
  Serial.begin(9600);
  
  while (!Serial);  
  Serial.println("LoRa Sender");
  if (!LoRa.begin(434E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  read_sensors();

  LoRa_send("MQ09", mq09);
  delay(100);
  LoRa_send("MQ135", mq135);
  delay(100);
  LoRa_send("DHT11", dht11);
  delay(100);
  LoRa_send("BMPP", bmp_pressure);
  delay(100);
  LoRa_send("BMPA", bmp_altitude);
  delay(100);
  LoRa_send("Light", lightintensity);
  delay(100);
  LoRa_send("PM", pm);
  delay(100);
  LoRa_send("Water", water);
  delay(5100);
 
}