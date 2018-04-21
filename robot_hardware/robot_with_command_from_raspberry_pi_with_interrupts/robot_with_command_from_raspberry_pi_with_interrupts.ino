#define TRUE 1
#define FALSE 0

// for left motor, HIGH is forward, LOW is backward
#define LEFT_FORWARD 0
#define LEFT_BACKWARD 1
#define PI 3.14115
// for right motor, LOW is forward, HIGH is backward
#define RIGHT_FORWARD 1
#define RIGHT_BACKWARD 0

// defines pins numbers
const int left_step_pin = 4;
const int left_dir_pin = 3;
const int right_step_pin = 6;
const int right_dir_pin = 5;

// hardware specs
// all measurements in cm
const double wheel_diameter = 95.05;
const double wheel_circumference = wheel_diameter * 3.1415;
const double robot_motor_to_motor_distance = 10;
const double robot_motor_radius_of_turn = robot_motor_to_motor_distance/2;
// some MAGIC numbers from trial and error
// calculations?
int turn_90_degree_steps = 200;
int quarter_wheel_step_number = 50;
int whole_wheel_step_number = 200;

// Interrupt pins + global variables

const int interrupt_pin_from_raspberry_pi = 21;
// mega interrupt pin is 21
// uno interrupt pin is 2
bool after_isr = FALSE;
String command;

void setup() {
  attachInterrupt(digitalPinToInterrupt(interrupt_pin_from_raspberry_pi), ISR_deal_with_new_serial, RISING);

  // stepper motor
  // 2 pins to arduino per stepper motor
  pinMode(left_step_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(right_step_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  Serial.begin(9600);
}
void loop() {
  if (after_isr)
    process_command(command);
  after_isr = FALSE;
}

void process_command(String incoming_command) {
  if (incoming_command == "forward") {
    go_forward(100);
  }
  else if (incoming_command == "rotateleft") {
    rotate_left();
  }
  else if (incoming_command == "rotateright") {
    rotate_right();
  }
}

void ISR_deal_with_new_serial() {
  Serial.println("in ISR");
  if (Serial.available() > 0) {
    command = Serial.readString();
  }
  after_isr = TRUE;
}

/* want to know how many times to turn the wheel to cover the requested distance
200 step motor, so each revolution (turn) of the wheel is 200 steps
degree turned per step = 1.8 degrees
steps/revolution = 200 = 360 degrees / (1.8 degrees/step)
distance/ circumference = number of revolutions
number of steps = number of revolutions * steps/revolution
*/

int convert_cm_to_steps(double distance_in_cm) {
  return (int) distance_in_cm / wheel_circumference * 200;
}

