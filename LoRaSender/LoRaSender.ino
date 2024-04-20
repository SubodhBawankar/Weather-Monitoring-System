#include <SPI.h>
#include <LoRa.h> 

float mq09 = 0;

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
  mq09 = analogRead(A5);
  LoRa.beginPacket();  
  LoRa.print(mq09);
  LoRa.endPacket();
  
  delay(1000);
 
}
