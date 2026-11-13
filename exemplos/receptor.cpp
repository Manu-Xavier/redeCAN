#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include <avr/wdt.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("------- Leitura CAN ----------");
  Serial.println("ID  DLC   DATA");

  wdt_enable(WDTO_4S);
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    /*Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");*/
    Serial.println(); Serial.println(); Serial.println(); Serial.println();
    Serial.println("------------------------------");
    if(canMsg.data[0] == 1) Serial.println("Porta-mala aberto");

    else Serial.println("Porta-mala fechado");
    
    int16_t valorBruto = (int16_t)(canMsg.data[1] << 8) | canMsg.data[2];
    float temperatura = valorBruto/10;
    Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");

    float tensaoBateria = canMsg.data[3] / 10;
    Serial.print("Tensao da bateria: "); Serial.print(tensaoBateria); Serial.println(" V");

    uint8_t hora = canMsg.data[4];
    uint8_t minuto = canMsg.data[5];
    uint8_t segundo = canMsg.data[6];

    Serial.print("Relogio: "); Serial.print(hora); Serial.print(":"); Serial.print(minuto); Serial.print(":"); Serial.println(segundo);

    Serial.println("------------------------------");        
  }

  wdt_reset();
}