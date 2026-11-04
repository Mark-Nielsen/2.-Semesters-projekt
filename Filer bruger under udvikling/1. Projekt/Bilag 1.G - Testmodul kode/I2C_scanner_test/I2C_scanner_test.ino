#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial); // Vent på at Serial Monitor åbner
  
  Serial.println("\n--- Starter Standard I2C Scanner ---");
  
  // Tvinger ESP32'eren til at bruge de korrekte standard-pins
  // SDA = 21, SCL = 22
  Wire.begin(21, 22); 
}

void loop() {
  byte error, address;  
  int nDevices = 0;

  Serial.println("Scanner I2C-bussen...");

  // Scanner alle mulige I2C-adresser fra 1 til 127
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("SUCCESS: Fandt I2C-enhed på adresse 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("FEJL: Ukendt fejl på adresse 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }    
  }
  
  if (nDevices == 0) {
    Serial.println("RESULTAT: INGEN ENHEDER FUNDET. Tjek strøm, GND og at SDA/SCL ikke er byttet!\n");
  } else {
    Serial.println("RESULTAT: Scan færdig.\n");
  }
  
  delay(5000); // Vent 5 sekunder før næste scan
}