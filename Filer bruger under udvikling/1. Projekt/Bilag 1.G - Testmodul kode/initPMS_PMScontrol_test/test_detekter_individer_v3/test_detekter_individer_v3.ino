#include <Adafruit_MLX90640.h>

// --- Configuration ---
#define MUX_ADDR 0x70
Adafruit_MLX90640 adafruit_mlx;

// Pin Definitions for Relays (connected to BSX20 Transistors)
// Cam 0 -> Pin 17, Cam 1 -> Pin 25, Cam 2 -> Pin 26
const int RELAY_PINS[3] = {17, 25, 26};
bool occupancyStatus[3] = {false, false, false};

float mlx90640Frame[768];     
float backgroundFrames[3][768]; 
bool isCalibrated[3] = {false, false, false};

// Detection Settings
const float TEMP_THRESHOLD = 3.0;
const float MIN_HUMAN_TEMP = 30.0;
const int   MIN_CLUSTER_SIZE = 12; 

// Helper to switch TCA9548A channels
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);

  // Initialize Relay Pins
  for (int i = 0; i < 3; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW); // Lights start OFF
  }

  Wire.begin(21, 22); 
  Wire.setClock(100000); // Stable speed for long wires

  for (uint8_t i = 0; i < 3; i++) {
    tcaselect(i);
    delay(10); 
    Serial.print("Initializing Camera on Channel "); Serial.println(i);
    
    if (!adafruit_mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
      Serial.print("Camera "); Serial.print(i); Serial.println(" NOT FOUND!");
    } else {
      adafruit_mlx.setMode(MLX90640_CHESS);
      adafruit_mlx.setResolution(MLX90640_ADC_18BIT);
      adafruit_mlx.setRefreshRate(MLX90640_4_HZ);
    }
  }
  Serial.println("\n--- System Initialization Complete ---");
  Serial.println("Initial Calibration: Keep views clear for 10s...");
  Serial.println("Manual Control: Press '1', '2', or '3' to toggle relays.");
}

void loop() {
  // --- 1. Keyboard Control (Manual Override/Test) ---
  if (Serial.available() > 0) {
    char key = Serial.read();
    int idx = -1;
    if (key == '1') idx = 0;
    else if (key == '2') idx = 1;
    else if (key == '3') idx = 2;

    if (idx != -1) {
      bool currentState = digitalRead(RELAY_PINS[idx]);
      digitalWrite(RELAY_PINS[idx], !currentState);
      Serial.print(">> KEYBOARD TOGGLE: Relay "); Serial.print(idx);
      Serial.println(!currentState ? " ON" : " OFF");
    }
  }

  // --- 2. Thermal Camera Loop (Automatic Control) ---
  for (uint8_t i = 0; i < 3; i++) {
    tcaselect(i);
    
    // Get frame from camera
    if (adafruit_mlx.getFrame(mlx90640Frame) != 0) continue;

    // Handle Calibration (takes background snapshot after 10 seconds)
    if (!isCalibrated[i] && millis() > 10000) {
      for (int p = 0; p < 768; p++) {
        backgroundFrames[i][p] = mlx90640Frame[p];
      }
      isCalibrated[i] = true;
      Serial.print(">>> Cam "); Serial.print(i); Serial.println(" Calibrated.");
    }

    // Process Detection
    if (isCalibrated[i]) {
      int clusterCount = getConnectedClusterSize(i);
      bool currentlyOccupied = (clusterCount >= MIN_CLUSTER_SIZE);
      
      // Update Relay and Serial if status changes
      if (currentlyOccupied != occupancyStatus[i]) {
        occupancyStatus[i] = currentlyOccupied;
        
        // --- THE AUTOMATIC TOGGLE ---
        digitalWrite(RELAY_PINS[i], currentlyOccupied ? HIGH : LOW);
        
        Serial.print("AUTO EVENT: Cam "); Serial.print(i);
        Serial.print(currentlyOccupied ? " [DETECTED]" : " [CLEAR]");
        Serial.print(" -> Relay "); Serial.print(RELAY_PINS[i]);
        Serial.println(currentlyOccupied ? " ON" : " OFF");
      }
    }
  }
  delay(50); 
}

// Algorithm to find connected heat pixels
int getConnectedClusterSize(uint8_t camIdx) {
  int connectedPixels = 0;
  // Scanning interior pixels to avoid array boundary issues with neighbor check
  for (int y = 1; y < 23; y++) {
    for (int x = 1; x < 31; x++) {
      int p = y * 32 + x;
      float delta = mlx90640Frame[p] - backgroundFrames[camIdx][p];

      // Check if pixel is warmer than background AND meets human temp floor
      if (delta > TEMP_THRESHOLD && mlx90640Frame[p] > MIN_HUMAN_TEMP) {
        int neighbors = 0;
        // Simple 4-way neighbor check for connectivity
        if ((mlx90640Frame[p-1] - backgroundFrames[camIdx][p-1]) > (TEMP_THRESHOLD * 0.8)) neighbors++;
        if ((mlx90640Frame[p+1] - backgroundFrames[camIdx][p+1]) > (TEMP_THRESHOLD * 0.8)) neighbors++;
        if ((mlx90640Frame[p-32] - backgroundFrames[camIdx][p-32]) > (TEMP_THRESHOLD * 0.8)) neighbors++;
        if ((mlx90640Frame[p+32] - backgroundFrames[camIdx][p+32]) > (TEMP_THRESHOLD * 0.8)) neighbors++;

        if (neighbors >= 2) connectedPixels++;
      }
    }
  }
  return connectedPixels;
}