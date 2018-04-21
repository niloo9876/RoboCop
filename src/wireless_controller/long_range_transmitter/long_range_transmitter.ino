/*
  Arduino Wireless Communication Tutorial
      Example 1 - Transmitter Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
const int led_pin = 10;
const int vrx_pin = A0;
const int vry_pin = A1;
const int sw_pin = 2;
typedef struct data_packet //Data Structure
{
  int    vry;// Initialize a storage place for the first integar that you wish to Receive
  int    vrx;// Initialize a storage place for the Second integar that you wish to Receive
  bool    sw;
};

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(led_pin, OUTPUT);
  pinMode(vrx_pin, INPUT);
  pinMode(vry_pin, INPUT);
  pinMode(sw_pin, INPUT);
   Serial.begin(9600);
}

void loop() {
  struct data_packet payload;
  payload.vrx = analogRead(vrx_pin);
  payload.vry = analogRead(vry_pin);
  payload.sw = !digitalRead(sw_pin); // negative logic, so negate to become positive logic
  radio.write(&payload, sizeof(payload));
  Serial.print(payload.vrx);
  Serial.print(",");
  Serial.print(payload.vry);
  Serial.print(",");
  Serial.println(payload.sw);
  //delay(50);
}
