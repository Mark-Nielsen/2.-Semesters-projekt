#pragma once
 
void uartInit(int baud_rate);
void uartSendString(const char *str);
void uartSendInt(int val);