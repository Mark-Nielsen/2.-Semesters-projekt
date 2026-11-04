#define RELAY_PIN 17  // Jeres valgte G17 pin

void setup() {
  // Initialiser seriel kommunikation med 115200 baud
  Serial.begin(115200);
  
  // Konfigurer relæ-pinden
  pinMode(RELAY_PIN, OUTPUT);
  
  // Start med relæet tændt - Relæet har her en Active-LOW logik
  digitalWrite(RELAY_PIN, LOW);
  
  Serial.println("--- Project Blackout: Relæ Kontrol ---");
  Serial.println("Skriv '0' for at TÆNDE relæet");
  Serial.println("Skriv '1' for at SLUKKE relæet");
}

void loop() {
  // Tjek om der er sendt data til terminalen
  // 21: Læs det indkommende tegn
  if (Serial.available() > 0) {
    char incomingByte = Serial.read(); 

    if (incomingByte == '0') {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println(">> Relæ: TÆNDT (Nødgenerator aktiv)");
    } 
    else if (incomingByte == '1') {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println(">> Relæ: SLUKKET (Nødgenerator afbrudt)");
    }
  }
}