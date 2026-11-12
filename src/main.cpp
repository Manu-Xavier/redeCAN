#include <Arduino.h>

static const int analogChannel = A0;

void setup(){
    Serial.begin(115200);
}

void loop(){
    Serial.print("Analog read: "); Serial.println(analogRead(analogChannel));

    delay(500);
}