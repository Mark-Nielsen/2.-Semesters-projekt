#include "uart.h"
#include <Arduino.h>

void uartInit(int baud_rate) {
    Serial.begin(baud_rate);
}

void uartSendString(const char *str) {
    Serial.println(str);
}

void uartSendInt(int val) {
    Serial.println(val);
}