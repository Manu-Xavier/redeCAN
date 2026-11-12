#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <mcp2515.h>
#include <avr/wdt.h>

/*
Identificação dos bytes do frame CAN:
    byte 0: porta-malas aberto ou fechado
    byte 1: byte 1 da temperatura
    byte 2: byte 2 da temperatura
    byte 3: tensão da bateria (dividir por 10 no receptor para achar o valor em volts corretamente)
    byte 4: nc
    byte 5: nc
    byte 6: nc
    byte 7: nc
*/

/*
    A4: SDA
    A5: SCL

    10: SS
    11: MOSI
    12: MISO
    13: SCK
*/

unsigned long refMillis = 0;
const unsigned int tempo = 100;
const uint8_t LM75B_Addr = 0x48;

const int led = 9; //led de debug
static const int portaMalas = 8;
static const int bateria = A0;

struct can_frame canMsg1;
//struct can_frame canMsg2;
MCP2515 mcp2515(10); //cria o objeto mcp2515 com pino SS configurado para o pino 10 do arduino

void enviaMsg1();
float lerTemp();
bool portaMalas_Status();
uint16_t tensaoBateria();

void setup() {
    //enviaMsg1();
  
    //while (!Serial); //Instrução valida apenas para embarcados que possuem comunicação usb nativa como o arduino leonardo, esp32 c3, etc.
                       //em microcontroladores que usam conversores serial/usb, é desnecessário. 
    Serial.begin(115200);

    Wire.begin();
    
    mcp2515.reset();
    mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();

    pinMode(led, OUTPUT);
    pinMode(portaMalas, INPUT_PULLUP);

    wdt_enable(WDTO_4S);
}

void loop() {
    if(millis() - refMillis >= tempo){
        enviaMsg1();

        Serial.println("Mensagem enviada");

        digitalWrite(led, !digitalRead(led));

        refMillis = millis();
    }

    wdt_reset();
}

void enviaMsg1(){
    int16_t temperatura = (int16_t)lroundf(lerTemp() * 10);

    canMsg1.can_id  = 0x01;
    canMsg1.can_dlc = 8;
    canMsg1.data[0] = portaMalas_Status();
    canMsg1.data[1] = temperatura >> 8;
    canMsg1.data[2] = temperatura & 0xFF;
    canMsg1.data[3] = tensaoBateria();
    canMsg1.data[4] = 0x00;
    canMsg1.data[5] = 0xFF;
    canMsg1.data[6] = 0xFF;
    canMsg1.data[7] = 0x00;

    mcp2515.sendMessage(&canMsg1);
}

float lerTemp() {
    Wire.beginTransmission(LM75B_Addr);
    Wire.write(0x00);
    Wire.endTransmission(false);
    Wire.requestFrom(LM75B_Addr, (uint32_t)2);

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t raw = (int16_t)((msb << 8) | lsb);

    return (raw >> 5) * 0.125f;
}

bool portaMalas_Status(){
    if(digitalRead(portaMalas) == 0)
        return 1;

    else
        return 0;
}

uint16_t tensaoBateria(){
    uint16_t leituraAnalogica = analogRead(bateria);

    uint8_t tensao = map(leituraAnalogica, 0, 1023, 100, 130);

    return tensao;
}