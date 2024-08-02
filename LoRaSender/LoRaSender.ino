#include <SPI.h>
#include <LoRa.h> 
#include <string.h>
#include <MQUnifiedsensor.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

/* 
DHT11         A0
MQ09          A1
PM            A2
MQ135         A3
BMP           A4 and A5

WaterSensor D1
LightSensor MUX with DHT11
Groove AQI Sensor NOT IN USE
*/

// Data Variables
float mq09_CO = 0;
float mq135_NH4 = 0; 
float dht11_temp = 0;
float dht11_hum = 0;
float t = 0;
float heat_index = 0;
float real_feel = 0;
float bmp_pressure = 0;
float bmp_altitude = 0;
// float lightintensity = 0;
float pm = 0;
float aqiPM = 0;
float aqiCO = 0;
float aqiNH = 0;
float aqiFinal = 0;

// PM Sensor Variable
int measurePin = A2; //PM Sensor Pins
int ledPower = 2;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;          
//
#define DHTPIN A0 
#define DHTTYPE DHT11   // DHT 11
#define WATER_SENSOR 1
// MQ Setting
MQUnifiedsensor MQ9("Arduino UNO", 5, 10, A1, "MQ-9");
MQUnifiedsensor MQ135("Arduino UNO", 5, 10, A3, "MQ-135");

Adafruit_BMP280 bmp; // I2C
DHT dht(DHTPIN, DHTTYPE);

void read_sensors(){
  // Reading PM Sensor
  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin); 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); 
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 170 * calcVoltage ;
  //
  
  // Reading MQ09 Sensor
  MQ9.update();
  MQ9.setA(599.65); MQ9.setB(-2.244); // Reading CO
  float CO = MQ9.readSensor(); 
  //Reading MQ135
  MQ135.update();
  MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configure the equation to calculate NH4 concentration value
  float NH4 = MQ135.readSensor(); 

  // Reading BMP Values
  bmp_pressure = bmp.readPressure() / 100;
  bmp_altitude = bmp.readAltitude(1013.25);
 
  //  
  mq09_CO  = CO;
  
  if (mq09_CO >= 0 && mq09_CO < 4.5) {
    aqiCO = map(mq09_CO, 0, 4.5, 0, 50);
  } else if (mq09_CO >= 4.5 && mq09_CO < 9.5) {
    aqiCO = map(mq09_CO, 4.5, 9.5, 51, 100);
  } else if (mq09_CO >= 9.5 && mq09_CO < 12.5) {
    aqiCO = map(mq09_CO, 9.5, 12.5, 101, 150);
  } else if (mq09_CO >= 12.5 && mq09_CO < 15.5) {
    aqiCO = map(mq09_CO, 12.5, 15.5, 151, 200);
  } else if (mq09_CO >= 15.5 && mq09_CO < 30.5) {
    aqiCO = map(mq09_CO, 15.5, 30.5, 201, 300);
  } else if (mq09_CO >= 30.5 && mq09_CO < 40.5) {
    aqiCO = map(mq09_CO, 30.5, 40.5, 301, 400);
  } else {
    aqiCO = map(mq09_CO, 40.5, 50.5, 401, 500);
  }

  mq135_NH4 = NH4;
  if (mq135_NH4 >= 0 && mq135_NH4 < 0.05) {
    aqiNH = (int)(mq135_NH4 / 0.05 * 50.0);
  } 
  else if (mq135_NH4 >= 0.05 && mq135_NH4 < 0.1) {
    aqiNH = (int)((mq135_NH4 - 0.05) / (0.1 - 0.05) * (100.0 - 51.0) + 51.0);
  } 
  else if (mq135_NH4 >= 0.1 && mq135_NH4 < 0.2) {
    aqiNH = (int)((mq135_NH4 - 0.1) / (0.2 - 0.1) * (150.0 - 101.0) + 101.0);
  } 
  else if (mq135_NH4 >= 0.2 && mq135_NH4 < 0.4) {
    aqiNH = (int)((mq135_NH4 - 0.2) / (0.4 - 0.2) * (200.0 - 151.0) + 151.0);
  } 
  else if (mq135_NH4 >= 0.4 && mq135_NH4 < 0.8) {
    aqiNH = (int)((mq135_NH4 - 0.4) / (0.8 - 0.4) * (300.0 - 201.0) + 201.0);
  } 
  else if (mq135_NH4 >= 0.8 && mq135_NH4 < 1.0) {
    aqiNH = (int)((mq135_NH4 - 0.8) / (1.0 - 0.8) * (400.0 - 301.0) + 301.0);
  } 
  else if (mq135_NH4 >= 1.0 && mq135_NH4 < 1.5) {
    aqiNH = (int)((mq135_NH4 - 1.0) / (1.5 - 1.0) * (500.0 - 401.0) + 401.0);
  } 
  else {
    aqiNH = 500;
  }

  pm = dustDensity;
  if (pm >= 0 && pm <= 50) {
        aqiPM = ((50.0 / 50.0) * pm);
    } else if (pm >= 51 && pm <= 100) {
        aqiPM = ((49.0 / 49.0) * (pm - 51.0)) + 51.0;
    } else if (pm >= 101 && pm <= 250) {
        aqiPM = ((99.0 / 149.0) * (pm - 101.0)) + 101.0;
    } else if (pm >= 251 && pm <= 350) {
        aqiPM = ((99.0 / 99.0) * (pm - 251.0)) + 201.0;
    } else if (pm >= 351 && pm <= 430) {
        aqiPM = ((99.0 / 80.0) * (pm - 351.0)) + 301.0;
    } else if (pm >= 431 ) {
        aqiPM = ((99.0 / 179.0) * (pm - 431.0)) + 401.0;
    }  else {
        aqiPM = -1; // Error value indicating out of range
    }
  
  aqiFinal = max(aqiPM, aqiCO);
  
  
  dht11_hum = dht.readHumidity();
  dht11_temp  = dht.readTemperature()  ;
  t= (dht11_temp * 1.8 ) + 32;
  heat_index =  (-42.379) + ((2.04901523) * t) + ((10.14333127) * dht11_hum) - ((0.22475541) * t * dht11_hum) - ((6.83783e-3) * t * t) - ((5.481717e-2) * dht11_hum * dht11_hum) + ((1.22874e-3) * t * t * dht11_hum) + ((8.5282e-4) * t * dht11_hum * dht11_hum) - ((1.99e-6) * t * t * dht11_hum * dht11_hum);
  real_feel = (heat_index - 32) * 5/9 ;


}

void LoRa_send(){
  String outputString = "";

  outputString += "A";
  outputString += String(mq09_CO, 2);
  outputString += "B";
  outputString += String(mq135_NH4, 2);
  outputString += "C";
  outputString += String(bmp_pressure, 2);
  outputString += "D";
  outputString += String(bmp_altitude, 2);
  outputString += "E";
  outputString += String(pm, 2);
  outputString += "F";
  outputString += "G";
  outputString += String(dht11_temp, 2);
  outputString += "H";
  outputString += String(dht11_hum, 2);
  outputString += "I";
  outputString += String(real_feel , 2);
  outputString += "J";
  outputString += String(aqiPM , 2);
  outputString += "K";
  outputString += String(aqiCO , 2);
  outputString += "L";
  //outputString += String(aqiNH , 2);
  outputString += "M";
  outputString += String(aqiFinal , 2);
  outputString += "N";


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

  // WaterSensor
  pinMode(WATER_SENSOR, INPUT);

  // MQ135 Setup
  MQ135.setRegressionMethod(1);
  MQ135.init(); 
  float calcR1 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ135.update();
    calcR1 += MQ135.calibrate(3.6);
  }
  MQ135.setR0(calcR1/10);
  //DHT11
  dht.begin();

  // BMP280 Setup
  unsigned status;status = bmp.begin();

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  //

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

