/*     Simple Stepper Motor Control Exaple Code

    by Dejan Nedelkovski, www.HowToMechatronics.com

*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

// for left motor, HIGH is forward, LOW is backward
#define LEFT_FORWARD 0
#define LEFT_BACKWARD 1

// for right motor, LOW is forward, HIGH is backward
#define RIGHT_FORWARD 1
#define RIGHT_BACKWARD 0
// defines pins numbers
const int left_step_pin = 4;
const int left_dir_pin = 3;
const int right_step_pin = 6;
const int right_dir_pin = 5;

typedef struct data_packet //Data Structure
{
  bool    forward;// Initialize a storage place for the first integar that you wish to Receive
  bool    backward;// Initialize a storage place for the Second integar that you wish to Receive
  bool left;
  bool right;
  bool    sw;
  bool center;
};

int turn_90_degree_steps = 200;
int quarter_wheel_step_number = 50;
int whole_wheel_step_number = 200;

void setup() {
  pinMode(left_step_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  //pinMode(4, OUTPUT);
  pinMode(right_step_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  digitalWrite(left_dir_pin, LOW);// LOW is forward, HIGH is backward
  digitalWrite(right_dir_pin, HIGH);// HIGH is forward, LOW is backward

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

  if (payload_received.center)
    dont_move();
  else if (payload_received.forward && !payload_received.right && !payload_received.left)
    go_forward();
  else if (payload_received.backward && !payload_received.right && !payload_received.left)
    go_backward();
  else if (payload_received.right && !payload_received.forward && !payload_received.backward)
    rotate_right();
  else if (payload_received.left && !payload_received.forward && !payload_received.backward)
    rotate_left();
  else if (payload_received.right && payload_received.forward)
    turn_right();
  else if (payload_received.left && payload_received.forward)
    turn_left();
  else
    dont_move();
  Serial.print(payload_received.forward);
  Serial.print(",");
  Serial.print(payload_received.backward);
  Serial.print(",");
  Serial.print(payload_received.left);
  Serial.print(",");
  Serial.print(payload_received.right);
  Serial.print(",");
  Serial.print(payload_received.center);
  Serial.print(",");
  Serial.println(payload_received.sw);
}

void go_straight() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  for (int i = 0; i < 100000; i++) {
    step_once_both();
    delay(5);
  }
}
void turn_left() {
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_single_right();
  delay(5);
}

void turn_right() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  step_single_left();
  delay(5);
}
void rotate_left() {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_once_both();
  delay(5);
}
void rotate_right() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_once_both();
  delay(5);
}
void forward_quarter_wheel() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_many_times(200);
}
void turn_right_90() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_many_times(turn_90_degree_steps);
}
void step_many_times(int number_steps) {
  for (int i = 0; i < number_steps; i++) {
    step_once_both();
    delay(5);
  }
}

void go_forward() {
  //Serial.println("going forward");
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_once_both();
  delay(5);
}

void go_backward() {
  Serial.println("going backward");
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_once_both();
  delay(5);
}

void step_once_both() {
  digitalWrite(right_step_pin, HIGH);
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
}
void step_single_left() {
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
}

void step_single_right() {
  digitalWrite(right_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  delayMicroseconds(500);
}
void dont_move() {
  ;
}
