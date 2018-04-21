/*
  CPEN 291 - 2017 Winter Term 2
  Demo: April 4th, 2018
  Group 13 9:00am

  code for arduino on remote control for manual mapping
  
  members:
  - Erick
  - Ming
  - Nancy
  - Niloo
  - Timothy
  - Yiyi
*/

#define TESTING 0
// wireless for rf module
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int deadzone_max = 580;
const int deadzone_min = 460;
RF24 radio(7, 8); // CE, CSN

bool collision_state = 0;

const byte addresses[][6] = {"10001", "10002"};
const int collision_led_pin = 6;
const int center_led_pin = 3;
const int not_center_led_pin = 5;

const int vrx_pin = A0;
const int vry_pin = A1;
const int sw_pin = 2;

const int peizo_pin = 4;

typedef struct data_packet //Data Structure
{
  int controller_state;
  bool sw_state;
};

void setup() {
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 10001
  radio.openReadingPipe(1, addresses[0]); // 10002
  radio.setPALevel(RF24_PA_MIN);

  pinMode(center_led_pin, OUTPUT);
  pinMode(not_center_led_pin, OUTPUT);
  pinMode(collision_led_pin, OUTPUT);
  pinMode(vrx_pin, INPUT);
  pinMode(vry_pin, INPUT);
  pinMode(sw_pin, INPUT);
  pinMode(peizo_pin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  delay(5);
  radio.stopListening();
  struct data_packet payload;
  int vry = analogRead(vry_pin);
  int vrx = analogRead(vrx_pin);

  // instead of holding the joystick in the normal orientation (0,0) on the left top corner and (x_max, y_max) in the right bottom corner
  // we rotate that orientation left by 90 degrees. so (x_max, 0) is the new left top corner and (0,y_max) is the new right bottom corner
  // in the center, aka dead zone. state 0
  if (vrx >= deadzone_min && vrx <= deadzone_max && vry >= deadzone_min && vry <= deadzone_max) {
    payload.controller_state = 0;
    center_led();
  }
  // in forward joystick position, state 1
  else if (vry > deadzone_min && vry < deadzone_max && vrx > deadzone_max){
    payload.controller_state = 1;
    not_center_led();
  }
  // in backward joystick position, state 2
  else if (vry > deadzone_min && vry < deadzone_max && vrx < deadzone_min){
    payload.controller_state = 2;
    not_center_led();
  }
  // in rotate left joystick position, state 3
  else if (vry < deadzone_min){
    payload.controller_state = 3;
    not_center_led();
  }
  // in rotate right joystick position, state 4
  else if (vry > deadzone_max){
    payload.controller_state = 4;
    not_center_led();
  }
  // code should never reach here. A BUG
  else {
    // BUG
  }
  // used a pull up resistor
  payload.sw_state = !digitalRead(sw_pin); // negative logic, so negate to become positive logic
  if (TESTING == 1) {
    Serial.print(payload.controller_state);
    Serial.print(payload.sw_state);
    Serial.println(collision_state);
    //delay(50);
  }
  // send data packet 
  radio.write(&payload, sizeof(payload));

  radio.startListening();
  if (radio.available()) {
    radio.read(&collision_state, sizeof(collision_state));
  }

  // buzz the piezo pin if the robot car has collided with an object
  if (collision_state == HIGH) {
    tone(peizo_pin, 500);
    digitalWrite(collision_led_pin, HIGH);
  }
  else {
    noTone(peizo_pin);
    digitalWrite(collision_led_pin, LOW);
  }
}

void center_led() {
  digitalWrite(center_led_pin, HIGH);
  digitalWrite(not_center_led_pin, LOW);
}

void not_center_led() {
  digitalWrite(not_center_led_pin, HIGH);
  digitalWrite(center_led_pin, LOW);
}

