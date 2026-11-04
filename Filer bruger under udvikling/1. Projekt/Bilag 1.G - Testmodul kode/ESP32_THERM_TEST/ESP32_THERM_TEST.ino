#include <Wire.h>
#include <Adafruit_MLX90640.h>

#define TCAADDR 0x70 // Multiplexerens adresse

Adafruit_MLX90640 mlx;
float mlx90640Frame[768];
float backgroundFrames[3][768]; // Baggrund for hvert af de 3 kameraer
bool isCalibrated[3] = {false, false, false};

// Funktion til at vælge kanal på multiplexeren (0, 1 eller 2)
void tcaSelect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  Serial.println("Initialiserer 3 kameraer via Multiplexer...");
// Skift til kanal i (SD0, SD1 eller SD2) 
  for (uint8_t i = 0; i < 3; i++) {
    tcaSelect(i); 
    delay(10);
    
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
      Serial.printf("FEJL: Kamera på kanal SD%d ikke fundet!\n", i);
    } else {
      mlx.setMode(MLX90640_CHESS);
      mlx.setRefreshRate(MLX90640_2_HZ);
      Serial.printf("Kamera SD%d OK!\n", i);
    }
  }
  Serial.println("System klar. Forlad lokalerne for kalibrering...");
}

void loop() {
  for (uint8_t cam = 0; cam < 3; cam++) {
    tcaSelect(cam); // Vælg kamera
    
    if (mlx.getFrame(mlx90640Frame) == 0) {
      
      // Kalibrering (efter 5 sekunder)
      if (!isCalibrated[cam] && millis() > 5000) {
        for (int i = 0; i < 768; i++) {
          backgroundFrames[cam][i] = mlx90640Frame[i];
        }
        isCalibrated[cam] = true;
        Serial.printf(">>> Baggrund gemt for Kamera SD%d\n", cam);
      }

      // Hvis kalibreret, tjek for personer
      if (isCalibrated[cam]) {
        checkOccupancy(cam);
      }
    }
    delay(100); // Lille pause før næste kamera
  }
}

void checkOccupancy(uint8_t camID) {
  int hotPixels = 0;
  for (int i = 0; i < 768; i++) {
    float delta = mlx90640Frame[i] - backgroundFrames[camID][i];
    // Samme logik som før: 3.5 grader varmere og over 28 grader
    if (delta > 3.5 && mlx90640Frame[i] > 28.0) {
      hotPixels++;
    }
  }

  Serial.printf("Cam SD%d | Varme Pixels: %d | STATUS: ", camID, hotPixels);
  if (hotPixels > 12) {
    Serial.println("[OPTAGET]");
  } else {
    Serial.println("[LEDIGT]");
  }
}