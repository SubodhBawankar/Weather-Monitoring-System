#include <SPI.h>
#include <LoRa.h> 
#include <string.h>
#include <MQUnifiedsensor.h>

/* 
MQ09 A1
DHT11 A0
LightSensor A3
WaterSensor D1
PM A4
BMP A4 ?? and A5
*/

// Data Variables
float mq09_CO = 0;
float mq09_CH4 = 0; 
float dht11_temp = 0;
float dht11_hum = 0;
float bmp_pressure = 0;
float bmp_altitude = 0;
float lightintensity = 0;
float pm = 0;
float water = 0; 

// PM Sensor Variable
int measurePin = A4; //PM Sensor Pins
int ledPower = 2;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;          
//

// MQ Setting
MQUnifiedsensor MQ9("Arduino UNO", 5, 10, A1, "MQ-9");


void read_sensors(){
  // Devangs Remaining Code

  // Reading PM Sensor
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); 
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 170 * calcVoltage - 0.1;
  //
  
  // Reading MQ09 Sensor
  MQ9.update();

  MQ9.setA(4269.6); MQ9.setB(-2.648); // Reading CH4
  float CH4 = MQ9.readSensor();
  MQ9.setA(599.65); MQ9.setB(-2.244); // Reading CO
  float CO = MQ9.readSensor(); 
 
  //  
  mq09_CO  = CO;
  mq09_CH4 = CH4;
  pm = dustDensity;
  
  dht11_hum = 10.10; // Update this
  dht11_temp = 10.10; // Update this

  bmp_pressure = 20.20;
  bmp_altitude = 30.30;
  lightintensity = 40.40;
  water = 100.13;
  delay(50);
}

void LoRa_send(){
  String outputString = "";

  outputString += "A ";
  outputString += String(mq09_CO, 2);
  outputString += "B ";
  outputString += String(mq09_CH4, 2);
  outputString += "C ";
  outputString += String(bmp_pressure, 2);
  outputString += "D ";
  outputString += String(bmp_altitude, 2);
  outputString += "E ";
  outputString += String(lightintensity, 2);
  outputString += "F ";
  outputString += String(pm, 2);
  outputString += "G ";
  outputString += String(water, 2);
  outputString += "H ";
  outputString += String(dht11_temp, 2);
  outputString += "I ";
  outputString += String(dht11_hum, 2);
  outputString += "J ";


  Serial.print("Value: "); 
  Serial.println(outputString); 

  LoRa.beginPacket();  
  LoRa.print(outputString);
  LoRa.endPacket();

}

void setup() {
  Serial.begin(9600);
  
  // PM Sensor Setup
  pinMode(ledPower,OUTPUT);

  // MQ09 Sensor Setup
  MQ9.setRegressionMethod(1);
  MQ9.init();
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ9.update();
    calcR0 += MQ9.calibrate(9.6);
  }
  MQ9.setR0(calcR0/10);


  while (!Serial);  
  Serial.println("Starting LoRa");
  if (!LoRa.begin(434E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  read_sensors();
  LoRa_send();
  delay(5000);
 
}


