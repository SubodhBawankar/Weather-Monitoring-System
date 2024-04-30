#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(11, 10);//10 RX pin of gps

void setup() {
  SoftSerial.begin(9600); // Initialize SoftwareSerial
  Serial.begin(9600);     // Initialize Serial
}

// Function to convert degrees and minutes to decimal format
float convertToDecimal(String value) {
  // Split degrees and minutes
  int dotIndex = value.indexOf('.');
  int degrees = value.substring(0, dotIndex - 2).toInt();
  float minutes = value.substring(dotIndex - 2).toFloat();
  // Convert minutes to decimal (divide by 60)
  float decimal = degrees + (minutes / 60);
  return decimal;
}

void loop() {
  if (SoftSerial.available()) {
    String sentence = SoftSerial.readStringUntil('\n'); // Read a sentence from SoftSerial

    // Check if the sentence is either $GPRMC or $GPGGA
    if (sentence.startsWith("$GPRMC")) {
      // Split the sentence into tokens separated by commas
      String tokens[15];
      int tokenIndex = 0;
      int startIndex = 0;
      int commaIndex = sentence.indexOf(',');
      while (commaIndex != -1) {
        tokens[tokenIndex++] = sentence.substring(startIndex, commaIndex);
        startIndex = commaIndex + 1;
        commaIndex = sentence.indexOf(',', startIndex);
      }
      tokens[tokenIndex++] = sentence.substring(startIndex);

      // Extract latitude, latitude direction, longitude, and longitude direction
      String latitude = tokens[3];
      String latitudeDir = tokens[4];
      String longitude = tokens[5];
      String longitudeDir = tokens[6];
      
      // Convert latitude and longitude to decimal format
      float latitudeDecimal = convertToDecimal(latitude);
      float longitudeDecimal = convertToDecimal(longitude);
      
      // Print latitude and longitude in decimal format
      Serial.print(latitudeDecimal, 6); // Print latitude with 6 decimal places
      Serial.print(",");
      Serial.println(longitudeDecimal, 6); // Print longitude with 6 decimal places
    }
  }
}
