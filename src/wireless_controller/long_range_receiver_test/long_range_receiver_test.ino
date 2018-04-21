#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

typedef struct data_packet //Data Structure
{
  int controller_state;
  bool sw_state;
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  struct data_packet payload_received;
  if (radio.available()) {
    radio.read(&payload_received, sizeof(payload_received));
  }
  Serial.print("Mode is: ");
  Serial.println(payload_received.controller_state);
  Serial.print("Switch is: ");
  Serial.println(payload_received.sw_state);
}
