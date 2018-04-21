// debug
#define TESTING 0

#define TRUE 1
#define FALSE 0

// for left motor, HIGH is forward, LOW is backward
#define LEFT_FORWARD 0
#define LEFT_BACKWARD 1
#define PI 3.14115
// for right motor, LOW is forward, HIGH is backward
#define RIGHT_FORWARD 0
#define RIGHT_BACKWARD 1

// movement types
#define MOVE_STRAIGHT 1
#define ROTATE  2
#define STOP    3
#define TURN    4

// direction of movement type
#define FORWARD 1
#define BACKWARD 0
#define RIGHT 1
#define LEFT 0


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
const double robot_motor_radius_of_turn = robot_motor_to_motor_distance / 2;
// some MAGIC numbers from trial and error
// calculations?
int turn_90_degree_steps = 200;
int quarter_wheel_step_number = 50;
int whole_wheel_step_number = 200;

String command;
void setup() {
  // stepper motor
  // 2 pins to arduino per stepper motor
  pinMode(left_step_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(right_step_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  Serial.begin(9600);
}
void loop() {
  go_forward(600);
  delay(1000);
  go_backward(600);
  delay(1000);
  rotate_left_degrees(90);
  delay(1000);
  rotate_right_degrees(90);
  delay(1000);
  dont_move();
  delay(1000);
}
/*
   input: 6 digit decimal number (int)
   output: nothing
   action: decode the command and move the robot
*/
void process_command(String incoming_command) {
  /* 6 digit int
    most significant digit is movement type
    one of: (1) move straight (2) rotate (3) stop (4) turn
    second most significant digit is direction of the movment
    one of: (0) left or backward (1) right or forward
  */
  int command_int = incoming_command.toInt();
  int movement_type = command_int / 100000;
  int direction_of_movement = (command_int / 10000) % 10;
  int data_value = command_int % 10000;

  switch (movement_type) {
    case MOVE_STRAIGHT:
      if (direction_of_movement == FORWARD)
        go_forward(data_value);
      else if (direction_of_movement == BACKWARD)
        go_backward(data_value);
      break;
    case ROTATE:
      if (direction_of_movement == RIGHT)
        rotate_right_degrees(data_value);
      else if (direction_of_movement == LEFT)
        rotate_left_degrees(data_value);
      break;
    case STOP:
      dont_move();
      break;
    /*case TURN:
      if (direction_of_movement == RIGHT)
        turn_right_degrees(data_value);
      else if (direction_of_movement == LEFT)
        turn_left(data_value);*/
    default:
      dont_move();
      break;
  }
  Serial.println("done");
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

void go_forward(int distance_in_cm) {
  int total_steps = convert_cm_to_steps(distance_in_cm);
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  for (int i = 0; i < total_steps; i++) {
    step_once_both();
    delay(5);
  }
}
void go_backward(int distance_in_cm) {
  int total_steps = convert_cm_to_steps(distance_in_cm);
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  for (int i = 0; i < total_steps; i++) {
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

void rotate_right_degrees(int degrees_to_turn) {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  int steps_to_turn = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  step_many_times(steps_to_turn);
}
void rotate_right() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_once_both();
  delay(5);
}

void rotate_left_degrees(int degrees_to_turn) {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  int steps_to_turn = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  step_many_times(steps_to_turn);
}

void rotate_right_90() {
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
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_once_both();
  delay(5);
}

void go_backward() {
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
