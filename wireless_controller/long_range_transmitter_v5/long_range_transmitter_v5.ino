

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int deadzone_max = 580;
const int deadzone_min = 460;
RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
const int led_pin = 10;
const int vrx_pin = A0;
const int vry_pin = A1;
const int sw_pin = 9;

typedef struct data_packet //Data Structure
{
  int controller_state;
  bool sw_state;
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
  //Serial.print("X--------------");
  //Serial.println(digitalRead(sw_pin));
  struct data_packet payload;
  int vry = analogRead(vry_pin);
  int vrx = analogRead(vrx_pin);
  /*
  Serial.print("vrx is: ");
  Serial.print(vrx);
  Serial.print(", vry is: ");
  Serial.println(vry);*/
  
  // in the center, aka dead zone. state 0
  if (vrx >= deadzone_min && vrx <= deadzone_max && vry >= deadzone_min && vry <= deadzone_max)
    payload.controller_state = 0;

  // in forward joystick position, state 1
  else if (vry > deadzone_min && vry < deadzone_max && vrx > deadzone_max)
    payload.controller_state = 1;

  // in backward joystick position, state 2
  else if (vry > deadzone_min && vry < deadzone_max && vrx < deadzone_min)
    payload.controller_state = 2;

  // in rotate left joystick position, state 3
  else if (vry < deadzone_min)
    payload.controller_state = 3;

  // in rotate right joystick position, state 4
  else if (vry > deadzone_max)
    payload.controller_state = 4;

  // code should never reach here. A BUG
  else {
    // BUG
  }
  
  payload.sw_state = !digitalRead(sw_pin); // negative logic, so negate to become positive logic
  Serial.print("Mode is: ");
  Serial.println(payload.controller_state);
  Serial.print("Switch is: ");
  Serial.println(payload.sw_state);
  radio.write(&payload, sizeof(payload));
}
