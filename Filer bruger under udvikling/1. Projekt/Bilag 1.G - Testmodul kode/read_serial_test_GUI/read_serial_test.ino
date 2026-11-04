#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "switch.h"
#include "uart.h"


int main(void)
{
    initSwitchPort();

    uartInit(9600); // 9600 baud @16MHz

    while (1)
    {
        int value = -1;
        const char* strValue = NULL;

        if (switchOn(0))
            value = 0;

        else if (switchOn(1))
            value = 50;


        else if (switchOn(7))
            strValue = "ok";

        else if (switchOn(6))
            strValue = "fejl";

        else if (switchOn(5))
            strValue = "idle";

        else if (switchOn(4))
            strValue = "sensor";

        if (value != -1)
        {
            uartSendInt(value);
            _delay_ms(300);
        }
        else if (strValue != NULL)
        {
            uartSendString(strValue);
            _delay_ms(300);
        }
    }
}

