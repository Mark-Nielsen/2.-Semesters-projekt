String msg = "";

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            msg.trim();
            if (msg.length() > 0) {
                Serial.println("Received: " + msg);
            }
            msg = "";
        } else if (c != '\r') {  // ignore carriage return
            msg += c;
        }
    }
}