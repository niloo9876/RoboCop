/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

typedef struct data_packet //Data Structure
{
  int    vry;// Initialize a storage place for the first integar that you wish to Receive
  int    vrx;// Initialize a storage place for the Second integar that you wish to Receive
  bool    sw;
};

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    struct data_packet payload_received;
    radio.read(&payload_received, sizeof(payload_received));
    Serial.print(payload_received.vrx);
    Serial.print(",");
    Serial.print(payload_received.vry);
    Serial.print(",");
    Serial.println(payload_received.sw);
  }
}
